#include "gaussianblurscene.h"

#include <axisalignedboundingbox.h>
#include <fullscreenquad.h>
#include <material.h>
#include <mesh.h>
#include <objloader.h>

#include <QGLWidget>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_2_Core>

GaussianBlurScene::GaussianBlurScene( const QString& fileName, QObject* parent )
    : AbstractScene( parent ),
      m_funcs( 0 ),
      m_textureSize( 512, 512 ),
      m_fileName( fileName ),
      m_mesh( 0 ),
      m_theta( 0.0f ),
      m_fsQuad( 0 ),
      m_pass( 1 ),
      m_blurPasses( 1 )
{
}

void GaussianBlurScene::initialise()
{
    // Resolve the OpenGL 3.2 functions that we need for instanced rendering
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_2_Core>();
    if ( !m_funcs )
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();

    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( false );
    loader.setTangentGenerationEnabled( false );
    loader.load( m_fileName );

    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/phong.vert",
                          ":/shaders/phong.frag" );

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Create a material that performs Ggaussian blur on the input texture
    m_postProcMaterial1 = createPostProcessMaterial1( ":/shaders/gaussianblur.vert",
                                                      ":/shaders/gaussianblur.frag" );
    m_postProcMaterial2 = createPostProcessMaterial2( m_postProcMaterial1 );

    // Create a full-screen quad and set the first material on it
    m_fsQuad = new FullScreenQuad( this );
    m_fsQuad->setMaterial( m_postProcMaterial1 );
    m_fsQuad->create();

    // Create and prepare the framebuffer object (FBO)
    prepareFBO1( m_postProcMaterial1->textureUnitConfiguration( 0 ).texture()->textureId() );
    prepareFBO2( m_postProcMaterial2->textureUnitConfiguration( 0 ).texture()->textureId() );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void GaussianBlurScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    // Get the maximum extent of the mesh so that we can scale it down to unit size
    // and rotate it
    float maxExtent = m_mesh->boundingVolume()->maxExtent();
    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 1.0f / maxExtent );

    m_theta -= 0.5f;
    if ( m_theta < 0.0f )
        m_theta += 360.0f;
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    static int frame = 0;
    static int delta = 1;
    if ( ++frame % 100 == 0 )
    {
        // Adjust the number of blur passes
        if ( m_blurPasses <= 0 )
            delta = 1;
        else if ( m_blurPasses >= 5 )
            delta = -1;
        m_blurPasses += delta;
        qDebug() << "Blur Passes =" << m_blurPasses;
    }
}

void GaussianBlurScene::render()
{
    // Direct rendering to the FBO
    if ( m_blurPasses > 0 )
        switchToFBO( m_fbo1 );

    // Draw the mesh
    renderScene();

    if ( m_blurPasses > 0 )
    {
        // Now direct rendering to the second FBO
        switchToFBO( m_fbo2 );

        // Post-process the scene by drawing a full-screen quad that is
        // textured using the texture attached to the FBO to which we previously
        // rendered the scene. The shaders used in this pass perform the
        // image filtering
        int pass = 1;
        m_fsQuad->setMaterial( m_postProcMaterial1 );
        postProcessScene( pass ); // vertical blur

        if ( m_blurPasses > 1 )
        {
            for ( int i = 0; i < m_blurPasses - 1; ++i )
            {
                switchToFBO( m_fbo1 );
                m_fsQuad->setMaterial( m_postProcMaterial2 );
                postProcessScene( ++pass ); // horizontal blur

                switchToFBO( m_fbo2 );
                m_fsQuad->setMaterial( m_postProcMaterial1 );
                postProcessScene( --pass ); // vertical blur
            }
        }

        // Reset rendering back to default frame buffer
        switchToDefaultFramebuffer();

        // Swap the material of the full-screen quad and execute th
        // 2nd post-processing pass
        m_fsQuad->setMaterial( m_postProcMaterial2 );
        postProcessScene( ++pass ); // horizontal blur
    }
}

void GaussianBlurScene::switchToFBO( GLuint fbo )
{
    // Bind the FBO
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, fbo );
}

void GaussianBlurScene::switchToDefaultFramebuffer()
{
    // Bind the default frame buffer
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GaussianBlurScene::renderScene()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();

    // Set up the view matrix
    QVector3D eye( 1.0f, 0.0f, 0.3f );
    QVector3D center( 0.0f, 0.0f, 0.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );

    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "Kd", QVector3D( 0.0f, 0.5f, 1.0f ) );
    shader->setUniformValue( "Ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "Ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "shininess", 100.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();
}

void GaussianBlurScene::postProcessScene( int pass )
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

void GaussianBlurScene::resize( int w, int h )
{
    qDebug() << Q_FUNC_INFO << "w =" << w << " h =" << h;

    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Resize the textures associated with our FBOs
    TexturePtr texture = m_postProcMaterial1->textureUnitConfiguration( 0 ).texture();
    texture->bind();
    texture->initializeToEmpty( QSize( w, h ) );

    texture = m_postProcMaterial2->textureUnitConfiguration( 0 ).texture();
    texture->bind();
    texture->initializeToEmpty( QSize( w, h ) );

    // Resize the depth render buffer associated with the first FBO too
    m_funcs->glBindRenderbuffer( GL_RENDERBUFFER, m_depthBuffer );
    m_funcs->glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h );

    // Save the size so we can set it as a uniform on the post-processing shader
    m_windowSize = QSize( w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 60.0f, aspect, 0.1f, 1024.0f );
}

void GaussianBlurScene::prepareFBO1( GLuint textureId )
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

void GaussianBlurScene::prepareFBO2( GLuint textureId )
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

MaterialPtr GaussianBlurScene::createPostProcessMaterial1( const QString& vertexShader,
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

    // We use GL_NEAREST as we don't want the texture unit doing any
    // filtering for us. The post-processing shader will do it explicitly
    sampler->setMinificationFilter( GL_NEAREST );
    sampler->setMagnificationFilter( GL_NEAREST );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
#else
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );

    // We use GL_NEAREST as we don't want the texture unit doing any
    // filtering for us. The post-processing shader will do it explicitly
    texture0->setMinificationFilter( GL_NEAREST );
    texture0->setMagnificationFilter( GL_NEAREST );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}

MaterialPtr GaussianBlurScene::createPostProcessMaterial2( const MaterialPtr& material1 )
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
    // Use same texture sampling configuration as before
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_NEAREST );
    texture0->setMagnificationFilter( GL_NEAREST );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}
