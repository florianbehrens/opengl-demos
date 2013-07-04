#include "edgedetectionscene.h"

#include <axisalignedboundingbox.h>
#include <fullscreenquad.h>
#include <material.h>
#include <mesh.h>
#include <objloader.h>

#include <QGLWidget>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_2_Core>

EdgeDetectionScene::EdgeDetectionScene( const QString& fileName, QObject* parent )
    : AbstractScene( parent ),
      m_funcs( 0 ),
      m_textureSize( 512, 512 ),
      m_fileName( fileName ),
      m_mesh( 0 ),
      m_theta( 0.0f ),
      m_fsQuad( 0 ),
      m_edgeThreshold( 0.1f )
{
}

void EdgeDetectionScene::initialise()
{
    // Resolve the OpenGL 3.2 functions that we need for instanced rendering
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_2_Core>();
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

    // Create a material that performs edge detection on the input texture
    MaterialPtr postProcessMaterial = createPostProcessMaterial( ":/shaders/edgedetect.vert",
                                                                 ":/shaders/edgedetect.frag" );

    // Create a cube and set the material on it
    m_fsQuad = new FullScreenQuad( this );
    m_fsQuad->setMaterial( postProcessMaterial );
    m_fsQuad->create();

    // Create and prepare the framebuffer object (FBO)
    prepareFBO( postProcessMaterial->textureUnitConfiguration( 0 ).texture()->textureId() );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void EdgeDetectionScene::update( float t )
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
}

void EdgeDetectionScene::render()
{
    // Direct rendering to the FBO
    switchToFBO();

    // Draw the mesh
    renderScene();

    // Reset rendering back to default frame buffer
    switchToDefaultFramebuffer();

    // Post-process the scene by drawing a full-screen quad that is
    // textured using the texture attached to the FBO to which we previously
    // rendered the scene. The shaders used in this pass perform the
    // image filtering
    postProcessScene();
}

void EdgeDetectionScene::switchToFBO()
{
    // Bind the FBO
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
}

void EdgeDetectionScene::switchToDefaultFramebuffer()
{
    // Bind the default frame buffer
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void EdgeDetectionScene::renderScene()
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

void EdgeDetectionScene::postProcessScene()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Bind our post-processing material (the shaders and texture with scene rendered to it)
    m_fsQuad->material()->bind();

    // Set up any uniforms needed by the post-processing shaders
    QOpenGLShaderProgramPtr shader = m_fsQuad->material()->shader();
    shader->setUniformValue( "edgeThreshold", m_edgeThreshold );
    shader->setUniformValue( "width", static_cast<float>( m_windowSize.width() ) );
    shader->setUniformValue( "height", static_cast<float>( m_windowSize.height() ) );

    // Let the mesh setup the remainder of its state and draw itself
    m_fsQuad->render();
}

void EdgeDetectionScene::resize( int w, int h )
{
    qDebug() << Q_FUNC_INFO << "w =" << w << " h =" << h;

    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Resize the texture associated with our FBO
    TexturePtr texture = m_fsQuad->material()->textureUnitConfiguration( 0 ).texture();
    texture->initializeToEmpty( QSize( w, h ) );

    // Resize the depth render buffer associated with the FBO too
    m_funcs->glBindRenderbuffer( GL_RENDERBUFFER, m_depthBuffer );
    m_funcs->glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h );

    // Save the size so we can set it as a uniform on the post-processing shader
    m_windowSize = QSize( w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 60.0f, aspect, 0.1f, 1024.0f );
}

void EdgeDetectionScene::prepareFBO( GLuint textureId )
{
    // Create and bind the FBO
    m_funcs->glGenFramebuffers( 1, &m_fbo );
    m_funcs->glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

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

MaterialPtr EdgeDetectionScene::createPostProcessMaterial( const QString& vertexShader,
                                                           const QString& fragmentShader )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( vertexShader,
                          fragmentShader );

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
    sampler->setMinificationFilter( GL_NEAREST );
    sampler->setMagnificationFilter( GL_NEAREST );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
#else
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_NEAREST );
    texture0->setMagnificationFilter( GL_NEAREST );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}
