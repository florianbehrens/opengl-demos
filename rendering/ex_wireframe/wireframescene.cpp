#include "wireframescene.h"

#include "axisalignedboundingbox.h"
#include "camera.h"
#include "mesh.h"
#include "objloader.h"

#include <QObject>

#include <math.h>

WireframeScene::WireframeScene( const QString& fileName, QObject* parent )
    : AbstractScene( parent ),
      m_fileName( fileName ),
      m_modelMatrix(),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_mesh( 0 ),
      m_theta( 0.0f ),
      m_time( 0.0f ),
      m_updatesEnabled( true )
{
    m_modelMatrix.setToIdentity();

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 1.4f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void WireframeScene::initialise()
{
    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( false );
    loader.setTangentGenerationEnabled( false );
    loader.load( m_fileName );

    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/wireframe.vert",
                          ":/shaders/wireframe.geom",
                          ":/shaders/wireframe.frag" );

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void WireframeScene::update( float t )
{
    Q_UNUSED( t );

    if ( m_updatesEnabled )
    {
        m_theta += 0.2;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;

        // Get the maximum extent of the object so that we can scale it down to unit size
        float maxExtent = m_mesh->boundingVolume()->maxExtent();

        m_modelMatrix.setToIdentity();
        m_modelMatrix.scale( 1.0f / maxExtent );
        m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    }

    // Store the time
    const float dt = t - m_time;
    m_time = t;

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( QVector3D( m_vx * dt, m_vy * dt, m_vz * dt ), option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle * dt );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle * dt );
        m_tiltAngle = 0.0f;
    }
}

void WireframeScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    shader->bind();

    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->projectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );
    shader->setUniformValue( "mvp", mvp );

    // We also need the viewport matrix (to transform into screen-space)
    shader->setUniformValue( "viewportMatrix", m_viewportMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the wireframe line properties
    shader->setUniformValue( "line.width", 0.4f );
    shader->setUniformValue( "line.color", QVector4D( 0.4f, 1.0f, 0.4f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "Kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
    shader->setUniformValue( "Ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "Ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "shininess", 100.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();
}

void WireframeScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the viewport matrix
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    m_viewportMatrix.setToIdentity();
    m_viewportMatrix.setColumn( 0, QVector4D( w2, 0.0f, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 1, QVector4D( 0.0f, h2, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 2, QVector4D( 0.0f, 0.0f, 1.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 3, QVector4D( w2, h2, 0.0f, 1.0f ) );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.1, 1024.0f );
}
