#include "geometryscene.h"
#include "cylinder.h"

#include <camera.h>

#include <QGLWidget>
#include <QImage>

GeometryScene::GeometryScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_cylinder( 0 ),
      m_theta( 0.0f ),
      m_rotateObject( false )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 10.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void GeometryScene::initialise()
{
    // Create a material that performs multi-texturing
    MaterialPtr material = createMaterial();

    // Create a cube and set the material on it
    m_cylinder = new Cylinder( this );
    m_cylinder->setMaterial( material );
    m_cylinder->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to white
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void GeometryScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    // Rotate the cube
    if ( m_rotateObject )
    {
        m_theta += 0.5f;
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

void GeometryScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_cylinder->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cylinder->material()->shader();

    // Set the usual view/projection matrices
    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;

    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );
    shader->setUniformValue( "mvp", mvp );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.ka", QVector3D( 0.1f, 0.1f, 0.3f ) );
    shader->setUniformValue( "material.kd", QVector3D( 0.0f, 0.0f, 0.9f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.4f, 0.4f, 0.4f ) );
    shader->setUniformValue( "material.shininess", 20.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_cylinder->render();
}

void GeometryScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 20.0f, aspect, 0.3, 1000.0f );
}

MaterialPtr GeometryScene::createMaterial()
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/phong.vert",
                          ":/shaders/phong.frag" );
    return material;
}
