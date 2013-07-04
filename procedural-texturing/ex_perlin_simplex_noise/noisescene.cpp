#include "noisescene.h"

#include <camera.h>
#include <sphere.h>

#include <QGLWidget>
#include <QImage>

NoiseScene::NoiseScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_obj( 0 ),
      m_theta( 0.0f ),
      m_rotate( false ),
      m_time( 0.0f )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 2.5f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void NoiseScene::initialise()
{
    // Create a material that performs multi-texturing
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/noise.vert",
                          ":/shaders/noise.frag" );

    // Create a sphere and set the material on it
    m_obj = new Sphere( this );
    m_obj->setMaterial( material );
    m_obj->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to (almost) white
    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
}

void NoiseScene::update( float t )
{
    // Store the time to pass into the shader
    m_time = t;

    // Rotate the cube
    if ( m_rotate )
    {
        m_theta += 0.5f;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    //m_modelMatrix.rotate( 90.0f, 1.0f, 0.0f, 0.0f );

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

void NoiseScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_obj->material()->bind();
    QOpenGLShaderProgramPtr shader = m_obj->material()->shader();

    // Set the mvp matrix
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;
    shader->setUniformValue( "mvp", mvp );
    shader->setUniformValue( "time", m_time );

    // Let the mesh setup the remainder of its state and draw itself
    m_obj->render();
}

void NoiseScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 1000.0f );
}
