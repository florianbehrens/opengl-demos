#include "earthscene.h"

#include <camera.h>
#include <sphere.h>

#include <QGLWidget>
#include <QImage>

EarthScene::EarthScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_sphere( 0 ),
      m_theta( 0.0f ),
      m_rotate( false )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 1.0f, 2.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void EarthScene::initialise()
{
#if !defined(Q_OS_MAC)
    // Create a material that performs multi-texturing
    MaterialPtr material = createMaterial(
                               "../../../opengl/assets/textures/earth/color.png",
                               "../../../opengl/assets/textures/earth/specular.png",
                               "../../../opengl/assets/textures/earth/nightlights.png" );
#else
    // Create a material that performs multi-texturing
    MaterialPtr material = createMaterial(
                               "../../../../../../opengl/assets/textures/earth/color.png",
                               "../../../../../../opengl/assets/textures/earth/specular.png",
                               "../../../../../../opengl/assets/textures/earth/nightlights.png" );
#endif

    // Create a cube and set the material on it
    m_sphere = new Sphere( this );
    m_sphere->setMaterial( material );
    m_sphere->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
}

void EarthScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    // Rotate the cube
    if ( m_rotate )
    {
        m_theta += 0.2f;
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

void EarthScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_sphere->material()->bind();
    QOpenGLShaderProgramPtr shader = m_sphere->material()->shader();

    // Set the usual view/projection matrices
    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;

    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );
    shader->setUniformValue( "mvp", mvp );

    // Set the lighting parameters - specify the direction in world coordinates
    // and transform to eye space as the shader expects
    QVector4D worldLightDirection( 4.0f, 2.0f, 1.5f, 0.0f );
    QVector4D eyeLightDirection = m_camera->viewMatrix() * worldLightDirection;
    shader->setUniformValue( "light.position", eyeLightDirection );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.ka", QVector3D( 0.01f, 0.01f, 0.01f ) );
    shader->setUniformValue( "material.kd", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.6f, 0.6f, 0.6f ) );
    shader->setUniformValue( "material.shininess", 5.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_sphere->render();
}

void EarthScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 1000.0f );
}

MaterialPtr EarthScene::createMaterial( const QString& colorFileName,
                                        const QString& specularFileName,
                                        const QString& nightlightsFileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/earth.vert",
                          ":/shaders/earth.frag" );

    // Create a diffuse color texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( colorFileName ) );
    texture0->generateMipMaps();

    // Create a specular texture map
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( specularFileName ) );
    texture1->generateMipMaps();

    // Create a nighttime lights texture
    TexturePtr texture2( new Texture( Texture::Texture2D ) );
    texture2->create();
    texture2->bind();
    texture2->setImage( QImage( nightlightsFileName ) );
    texture2->generateMipMaps();

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by many textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "dayColor" ) );
    material->setTextureUnitConfiguration( 1, texture1, sampler, QByteArrayLiteral( "specularMap" ) );
    material->setTextureUnitConfiguration( 2, texture2, sampler, QByteArrayLiteral( "nightColor" ) );
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

    texture2->bind();
    texture2->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture2->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture2->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture2->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "dayColor" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "specularMap" ) );
    material->setTextureUnitConfiguration( 2, texture2, QByteArrayLiteral( "nightColor" ) );
#endif

    return material;
}
