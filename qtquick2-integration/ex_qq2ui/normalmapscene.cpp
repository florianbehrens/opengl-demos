#include "normalmapscene.h"

#include <camera.h>
#include <mesh.h>
#include <objloader.h>

#include <QGLWidget>
#include <QImage>

#if !defined(Q_OS_MAC)
#include <QOpenGLFunctions_3_3_Compatibility>
#else
#include <QOpenGLFunctions_2_1>
#endif

/*
static void dumpTextureUnitConfiguration()
{
    GLint textureUnit = 0;
    glGetIntegerv( GL_ACTIVE_TEXTURE, &textureUnit );
    textureUnit -= GL_TEXTURE0;
    qDebug() << "Active Texture Unit =" << textureUnit;

    GLint textureUnitCount;
    glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnitCount );
    //glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnitCount );
    qDebug() << "Found" << textureUnitCount << "available texture units";

    for ( int i = 0; i < textureUnitCount; ++i )
    {
        glActiveTexture( GL_TEXTURE0 + i );
        GLint textureId = 0;
        glGetIntegerv( GL_TEXTURE_BINDING_2D, &textureId );
        qDebug() << "Unit: <<" << i << "Texture Id =" << textureId;
    }

    glActiveTexture( GL_TEXTURE0 + textureUnit );
}
*/

NormalMapScene::NormalMapScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_mesh( 0 ),
      m_theta( 0.0f ),
      m_rotate( false ),
      m_rotationSpeed( 0.5f ),
      m_funcs( 0 )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 2.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void NormalMapScene::initialise()
{
    //dumpTextureUnitConfiguration();

#if !defined(Q_OS_MAC)
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
#else
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_2_1>();
#endif
    if ( !m_funcs )
        qFatal( "Could not obtain functions object" );
    m_funcs->initializeOpenGLFunctions();

    // Create a material that performs multi-texturing
#if !defined(Q_OS_MAC)
    MaterialPtr material = createTextureMaterial( "../../assets/meshes/ogrehead/ogrehead_diffuse.png",
                                                  "../../assets/meshes/ogrehead/ogre_normalmap.png" );
#else
    MaterialPtr material = createTextureMaterial(
                               "../../../opengl/assets/meshes/ogrehead/ogrehead_diffuse.png",
                               "../../../opengl/assets/meshes/ogrehead/ogre_normalmap.png" );
#endif

    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( true );
    loader.setTangentGenerationEnabled( true );
#if !defined(Q_OS_MAC)
    loader.load( "../../assets/meshes/ogrehead/ogrehead.obj" );
#else
    loader.load( "../../../opengl/assets/meshes/ogrehead/ogrehead.obj" );
#endif

    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to (almost) white
    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );

    m_funcs->glActiveTexture( GL_TEXTURE1 );
    m_funcs->glBindTexture( GL_TEXTURE_2D, 0 );
    m_funcs->glActiveTexture( GL_TEXTURE0 );
    m_funcs->glBindTexture( GL_TEXTURE_2D, 0 );

    //dumpTextureUnitConfiguration();
}

void NormalMapScene::cleanup()
{
    delete m_mesh;
}

void NormalMapScene::update( float t )
{
    QMutexLocker locker( &m_mutex );

    // Nothing to do here
    Q_UNUSED( t );

    // Rotate the cube
    if ( m_rotate )
    {
        m_theta += m_rotationSpeed;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

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
}

void NormalMapScene::render()
{
    //dumpTextureUnitConfiguration();

    // Enable depth buffer writes (QQ2 disables them)
    glDepthMask( true );

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();

    {
        QMutexLocker locker( &m_mutex );

        // Set the usual view/projection matrices
        QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
        QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
        QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;

        shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
        shader->setUniformValue( "normalMatrix", normalMatrix );
        shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );
        shader->setUniformValue( "mvp", mvp );
    }

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 20.0f );

    // Let the mesh setup the remainder of its state and draw itself
#if !defined(Q_OS_MAC)
    m_mesh->render();
#else
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    m_mesh->bindBuffers();
    m_mesh->render();
    glPopClientAttrib();
#endif

    // Unbind shader
    shader->release();

    // Restore texture unit state
    //  - remove sampler objects
    //  - set active texture unit back to the first unit
#if !defined(Q_OS_MAC)
    m_funcs->glBindSampler( 0, 0 );
    m_funcs->glBindSampler( 1, 0 );
#endif
    m_funcs->glActiveTexture( GL_TEXTURE0 );

    //dumpTextureUnitConfiguration();
}

void NormalMapScene::resize( int w, int h )
{
    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 1000.0f );
}

MaterialPtr NormalMapScene::createTextureMaterial( const QString& diffuseFileName,
                                                   const QString& normalFileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
#if !defined(Q_OS_MAC)
    material->setShaders( ":/shaders/normalmap.vert",
                          ":/shaders/normalmap.frag" );
#else
    material->setShaders( ":/shaders/normalmap_2_1.vert",
                          ":/shaders/normalmap_2_1.frag" );
#endif

    // Create a diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( diffuseFileName ) );
    texture0->generateMipMaps();

    // Create a normal map
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( normalFileName ) );
    texture1->generateMipMaps();

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by many textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, sampler, QByteArrayLiteral( "texture1" ) );
#else
    texture0->bind();
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    texture1->bind();
    texture1->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture1->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture1->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture1->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "texture1" ) );
#endif

    return material;
}

void NormalMapScene::setRotationSpeed( float rotationSpeed )
{
    if ( qFuzzyCompare( rotationSpeed, m_rotationSpeed ) )
        return;
    m_rotationSpeed = rotationSpeed;
    emit rotationSpeedChanged();
}

float NormalMapScene::rotationSpeed() const
{
    return m_rotationSpeed;
}
