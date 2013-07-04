#include "proceduralscene.h"

#include <camera.h>
#include <plane.h>

#include <QGLWidget>
#include <QImage>

ProceduralScene::ProceduralScene( QObject* parent )
    : CameraScene( parent ),
      m_obj( 0 ),
      m_theta( 0.0f ),
      m_rotate( false ),
      m_time( 0.0f )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 3.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void ProceduralScene::initialise()
{
    // Create a material that performs multi-texturing
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/procedural.vert",
                          ":/shaders/procedural.frag" );

    // Create a cube and set the material on it
    m_obj = new Plane( this );
    m_obj->setMaterial( material );
    m_obj->setWidth( 1.0f );
    m_obj->setHeight( 1.0f );
    m_obj->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to (almost) white
    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
}

void ProceduralScene::update( float t )
{
    // Store the time to pass into the shader
    m_time = t;

    // Rotate the scene
    if ( m_rotate )
    {
        m_theta += 0.5f;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( 90.0f, 1.0f, 0.0f, 0.0f );
}

void ProceduralScene::render()
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

