#include "skyboxscene.h"

#include <skybox.h>

SkyBoxScene::SkyBoxScene( QObject* parent )
    : AbstractScene( parent ),
      m_theta( 0.0f ),
      m_phi( 0.0f ),
      m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_skyBox( new SkyBox( this ) ),
      m_yawSpeed( 0.0f ),
      m_pitchSpeed( 0.0f )
{
}

void SkyBoxScene::initialise()
{
    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Create a material to be shared by the skybox and reflective object
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/skybox.vert",
                          ":/shaders/skybox.frag" );

    // Create the skybox
    m_skyBox->setMaterial( material );
    m_skyBox->create();
#if !defined(Q_OS_MAC)
    m_skyBox->setCubeMapBaseName( "../../../opengl/assets/textures/cubemap_miramar/miramar" );
#else
    m_skyBox->setCubeMapBaseName( "../../../../../../opengl/assets/textures/cubemap_miramar/miramar" );
#endif

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
}

void SkyBoxScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );
    m_theta += m_yawSpeed;
    m_phi += m_pitchSpeed;
}

void SkyBoxScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set up the view matrix
    QVector3D eye( 0.0f, 0.0f, 0.0f );
    QVector3D center( 0.0f, 0.0f, 1.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );
    m_viewMatrix.rotate( m_phi, 1.0f, 0.0f, 0.0f );
    m_viewMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;

    // Draw stuff
    m_skyBox->render( mvp );
}

void SkyBoxScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 25.0f, aspect, 0.3f, 100.0f );
}

void SkyBoxScene::setYawSpeed( float yawSpeed )
{
    m_yawSpeed = yawSpeed;
}

void SkyBoxScene::setPitchSpeed( float pitchSpeed )
{
    m_pitchSpeed = pitchSpeed;
}
