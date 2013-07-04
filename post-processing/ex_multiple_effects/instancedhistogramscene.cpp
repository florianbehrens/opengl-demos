#include "instancedhistogramscene.h"

#include <camera.h>
#include <cube.h>
#include <fullscreenquad.h>
#include <torus.h>
#include <material.h>
#include <sphere.h>

#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFunctions_3_2_Core>
#if defined(Q_OS_MAC)
#include <QOpenGLExtensions>
#endif

#include <math.h>

static const int xPoints = 100;
static const int zPoints = 100;
static const int pointCount = xPoints * zPoints;

InstancedHistogramScene::InstancedHistogramScene( QObject* parent )
    : AbstractScene( parent ),
      m_funcs( 0 ),
#if defined(Q_OS_MAC)
      m_instanceFuncs( 0 ),
#endif
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_theta( 0.0f ),
      m_modelMatrix(),
      m_data( 3 * pointCount ),
      m_sphere( 0 ),
      m_sphereModelMatrix(),
      m_time( 0.0f ),
      m_updatesEnabled( true ),
      m_fsQuad( 0 ),
      m_blurPass( 1 ),
      m_textureSize( 512, 512 )
{
    m_modelMatrix.setToIdentity();
    update( 0.0f );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 25.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    m_camera->translateWorld( QVector3D( 0.0f, 7.0f, 0.0f ), Camera::DontTranslateViewCenter );
}

void InstancedHistogramScene::initialise()
{
    // Resolve the OpenGL functions that we need for instanced rendering
#if !defined(Q_OS_MAC)
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();
#else
    m_instanceFuncs = new QOpenGLExtension_ARB_instanced_arrays;
    if ( !m_instanceFuncs->initializeOpenGLFunctions() )
        qFatal( "Could not resolve GL_ARB_instanced_arrays functions" );

    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_2_Core>();
#endif
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();

    // Create a Material we can use to operate on instanced geometry
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/instancedhistogram.vert",
                          ":/shaders/instancedhistogram.frag" );

    // Create a cube
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Create a VBO ready to hold our data
    prepareVertexBuffers();

    // Tell OpenGL how to pass the data VBOs to the shader program
    prepareVertexArrayObject();

    // Use a sphere for the background - we'll render the scene
    // inside the sphere
    MaterialPtr noiseMaterial( new Material );
    noiseMaterial->setShaders( ":/shaders/noise.vert",
                               ":/shaders/noise.frag" );

    m_sphere = new Sphere( this );
    m_sphere->setMaterial( noiseMaterial );
    m_sphere->create();

    // Enable depth testing to prevent artifacts
    glEnable( GL_DEPTH_TEST );

    // Create a material that performs Gaussian blur on the input texture
    m_postProcBlurMaterial1 = createBlurMaterial1( ":/shaders/gaussianblur.vert",
                                                   ":/shaders/gaussianblur.frag" );
    m_postProcBlurMaterial2 = createBlurMaterial2( m_postProcBlurMaterial1 );

    // Create a material thet applies the television effect
    m_postProcTelevisionMaterial = createTelevisionMaterial( ":/shaders/television.vert",
                                                             ":/shaders/television.frag",
                                                             m_postProcBlurMaterial1 );

    // Create a full-screen quad and set the first material on it
    m_fsQuad = new FullScreenQuad( this );
    m_fsQuad->setMaterial( m_postProcBlurMaterial1 );
    m_fsQuad->create();

    // Create and prepare the framebuffer object (FBO)
    prepareFBO1( m_postProcBlurMaterial1->textureUnitConfiguration( 0 ).texture()->textureId() );
    prepareFBO2( m_postProcBlurMaterial2->textureUnitConfiguration( 0 ).texture()->textureId() );
}

void InstancedHistogramScene::update( float t )
{
    // Update the data being plotted
    if ( m_updatesEnabled )
    {
        const float xMin = -15.0f, xMax = 15.0f;
        const float zMin = -15.0f, zMax = 15.0f;
        const float dx = ( xMax - xMin ) / static_cast<float>( xPoints - 1 );
        const float dz = ( zMax - zMin ) / static_cast<float>( zPoints - 1 );
        int i = 0;
        const float A = 5.0;
        for ( int zi = 0; zi < zPoints; ++zi )
        {
            float z = zMin + static_cast<float>( zi ) * dz;

            for ( int xi = 0; xi < xPoints; ++xi )
            {
                float x = xMin + static_cast<float>( xi ) * dx;

                double r = sqrt( x * x + z * z );
                float y = A * ( sinf( 2.5f * t ) + cosf( 2.5f * t ) ) * j0( r );

                m_data[3 * i]     = x;
                m_data[3 * i + 1] = y;
                m_data[3 * i + 2] = z;
                ++i;
            }
        }
    }

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( QVector3D( m_vx, m_vy, m_vz ), option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle );
        m_tiltAngle = 0.0f;
    }

    // Store the time to pass into the shader
    m_time = t;

    m_sphereModelMatrix.setToIdentity();
    m_sphereModelMatrix.scale( 30.0f );
}

void InstancedHistogramScene::render()
{
    // Render to first fbo
    switchToFBO( m_fbo1 );
    renderScene();

    // Apply the blur
    switchToFBO( m_fbo2 );
    int pass = 1;
    m_fsQuad->setMaterial( m_postProcBlurMaterial1 );
    renderBlurEffect( pass ); // vertical blur
    switchToFBO( m_fbo1 );
    m_fsQuad->setMaterial( m_postProcBlurMaterial2 );
    renderBlurEffect( ++pass ); // horizontal blur

    // Apply television effect
    switchToDefaultFramebuffer();
    m_fsQuad->setMaterial( m_postProcTelevisionMaterial );
    renderTelevisionEffect();
}

void InstancedHistogramScene::switchToFBO( GLuint fbo )
{
    // Bind the FBO
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, fbo );
}

void InstancedHistogramScene::switchToDefaultFramebuffer()
{
    // Bind the default frame buffer
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void InstancedHistogramScene::renderScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );

    // Bind the shader program
    m_cube->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();

    // Calculate needed matrices
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );

    // Scale the x-z dimensions of the cuboids
    shader->setUniformValue( "cubeScale", QVector2D( 0.25f, 0.25f ) );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.shininess", 100.0f );

    // Draw the cuboids
    m_cube->vertexArrayObject()->bind();
    m_funcs->glDrawElementsInstanced( GL_TRIANGLES, m_cube->indexCount(), GL_UNSIGNED_INT, 0, pointCount );


    // Draw the background sphere
    m_sphere->material()->bind();
    shader = m_sphere->material()->shader();

    // Set the mvp matrix
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_sphereModelMatrix;
    shader->setUniformValue( "mvp", mvp );
    shader->setUniformValue( "time", m_time );

    // Let the mesh setup the remainder of its state and draw itself
    m_sphere->render();
}

void InstancedHistogramScene::renderBlurEffect( int pass )
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Bind our post-processing material (the shaders and texture with scene rendered to it)
    m_fsQuad->material()->bind();

    // Set up any uniforms needed by the post-processing shaders
    QOpenGLShaderProgramPtr shader = m_fsQuad->material()->shader();
    shader->setUniformValue( "pass", pass );
    shader->setUniformValue( "width", static_cast<float>( m_windowSize.width() ) );
    shader->setUniformValue( "height", static_cast<float>( m_windowSize.height() ) );

    // Let the mesh setup the remainder of its state and draw itself
    m_fsQuad->render();
}

void InstancedHistogramScene::renderTelevisionEffect()
{
    // Set the clear color to grey
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Bind our post-processing material (the shaders and texture with scene rendered to it)
    m_fsQuad->material()->bind();

    // Set up any uniforms needed by the post-processing shaders
    QOpenGLShaderProgramPtr shader = m_fsQuad->material()->shader();
    QVector2D resolution( static_cast<float>( m_windowSize.width() ),
                          static_cast<float>( m_windowSize.height() ) );
    shader->setUniformValue( "resolution", resolution );
    shader->setUniformValue( "time", m_time );

    // Let the mesh setup the remainder of its state and draw itself
    m_fsQuad->render();
}

void InstancedHistogramScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Resize the textures associated with our FBOs
    TexturePtr texture = m_postProcBlurMaterial1->textureUnitConfiguration( 0 ).texture();
    texture->bind();
    texture->initializeToEmpty( QSize( w, h ) );

    texture = m_postProcBlurMaterial2->textureUnitConfiguration( 0 ).texture();
    texture->bind();
    texture->initializeToEmpty( QSize( w, h ) );

    // Resize the depth render buffer associated with the first FBO too
    m_funcs->glBindRenderbuffer( GL_RENDERBUFFER, m_depthBuffer );
    m_funcs->glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h );

    // Save the size so we can set it as a uniform on the post-processing shader
    m_windowSize = QSize( w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 100.0f );
}

void InstancedHistogramScene::prepareVertexBuffers()
{
    // Populate the data buffer object
    m_dataBuffer.create();
    m_dataBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );
}

void InstancedHistogramScene::prepareVertexArrayObject()
{
    // Bind the marker's VAO
    m_cube->vertexArrayObject()->bind();

    // Enable the data buffer and add it to the marker's VAO
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    shader->bind();
    m_dataBuffer.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );

    // We only vary the point attribute once per instance
    GLuint pointLocation = shader->attributeLocation( "point" );
#if !defined(Q_OS_MAC)
    m_funcs->glVertexAttribDivisor( pointLocation, 1 );
#else
    m_instanceFuncs->glVertexAttribDivisorARB( pointLocation, 1 );
#endif
    m_cube->vertexArrayObject()->release();
}

void InstancedHistogramScene::prepareFBO1( GLuint textureId )
{
    // Create and bind the FBO
    m_funcs->glGenFramebuffers( 1, &m_fbo1 );
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, m_fbo1 );

    // Attach the texture object to the FBO as color attachment 0
    // The fragment shader will output to this
    m_funcs->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0 );

    // Create a depth buffer to be associated with the FBO
    m_funcs->glGenRenderbuffers( 1, &m_depthBuffer );
    m_funcs->glBindRenderbuffer( GL_RENDERBUFFER, m_depthBuffer );

    // We start off with a 512x512 texture. This gets re-allocated in the resize() function
    // along with the texture object storage
    m_funcs->glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_textureSize.width(), m_textureSize.height() );

    // Attach the depth buffer to the FBO
    m_funcs->glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer );

    // Setup targets for shader outputs
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    m_funcs->glDrawBuffers( 1, drawBuffers );

    // Make sure that the FBO is complete
    GLenum result = m_funcs->glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if ( result == GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer is complete";
    else
        qDebug() << "Framebuffer error: " << result;

    // Finally, unbind the FBO which resets to the default framebuffer
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void InstancedHistogramScene::prepareFBO2( GLuint textureId )
{
    // Create and bind the FBO
    m_funcs->glGenFramebuffers( 1, &m_fbo2 );
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, m_fbo2 );

    // Attach the texture object to the FBO as color attachment 0
    // The fragment shader will output to this
    m_funcs->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0 );

    // We don't need a depth buffer for the last pass as we are only drawing a single quad

    // Setup targets for shader outputs
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    m_funcs->glDrawBuffers( 1, drawBuffers );

    // Make sure that the FBO is complete
    GLenum result = m_funcs->glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if ( result == GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer is complete";
    else
        qDebug() << "Framebuffer error: " << result;

    // Finally, unbind the FBO which resets to the default framebuffer
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

MaterialPtr InstancedHistogramScene::createBlurMaterial1( const QString& vertexShader,
                                                          const QString& fragmentShader )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( vertexShader, fragmentShader );

    // Create a base diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();

    // We start off with a 512x512 texture. This gets re-allocated in the resize() function
    // along with the depth render buffer
    texture0->initializeToEmpty( m_textureSize );

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by both textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );

    // We use GL_LINEAR to optimise blur shader
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
#else
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );

    // We use GL_LINEAR to optimise blur shader
    texture0->setMinificationFilter( GL_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}

MaterialPtr InstancedHistogramScene::createBlurMaterial2( const MaterialPtr& material1 )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShader( material1->shader() );

    // Create a new texture to render into
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();

    // We start off with a 512x512 texture. This gets re-allocated in the resize() function
    // along with the depth render buffer
    texture0->initializeToEmpty( m_textureSize );

#if !defined(Q_OS_MAC)
    // Reuse Sampler from first material
    SamplerPtr sampler = material1->textureUnitConfiguration( 0 ).sampler();

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
#else
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}

MaterialPtr InstancedHistogramScene::createTelevisionMaterial( const QString& vertexShader,
                                                               const QString& fragmentShader,
                                                               const MaterialPtr& materialShare )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( vertexShader, fragmentShader );

    // Reuse Sampler and Texture from material passed in
    TexturePtr texture = materialShare->textureUnitConfiguration( 0 ).texture();
#if !defined(Q_OS_MAC)
    SamplerPtr sampler = materialShare->textureUnitConfiguration( 0 ).sampler();

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture, sampler, QByteArrayLiteral( "texture0" ) );
#else
    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture, QByteArrayLiteral( "texture0" ) );
#endif
    return material;
}
