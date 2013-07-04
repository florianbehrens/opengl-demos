#include "manyobjectsscene.h"

#include "axisalignedboundingbox.h"
#include "torus.h"
#include "sphere.h"
#include "cube.h"
#include "camera.h"

#include <QDateTime>
#include <QColor>

#include <math.h>

ManyObjectsScene::ManyObjectsScene( )
    : CameraScene(),
      m_modelMatrix(),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();
}

void ManyObjectsScene::initialise()
{
    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/basic.vert",
                          ":/shaders/basic.frag" );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 10.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    m_cube = new Cube;
    m_cube->setMaterial( material );
    m_cube->create();

    m_sphere = new Sphere;
    m_sphere->setRadius( 1.0 );
    m_sphere->setMaterial( material );
    m_sphere->create();

    m_torus = new Torus;
    m_torus->setMinorRadius( 0.5f );
    m_torus->setMaterial( material );
    m_torus->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void ManyObjectsScene::update( float t )
{
    Q_UNUSED( t );

    m_theta += 0.4;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
}

void ManyObjectsScene::setupMatrices(QOpenGLShaderProgramPtr shader, const QMatrix4x4& modelMatrix)
{
    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->viewProjectionMatrix() );
    shader->setUniformValue( "mvp", mvp );
}

void ManyObjectsScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_sphere->material()->shader();
    shader->bind();

    // Set the lighting parameters
    shader->setUniformValue( "lightPosition", QVector4D( 1.0f, 3.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "lightIntensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // For each object, set matrix uniforms (more on this later) and render

    // Cube
    setupMatrices(shader, m_modelMatrix);
    shader->setUniformValue( "kd", QVector3D(1.0, 0.0, 0.0) );
    m_sphere->render();

    // Cube
    m_modelMatrix.translate(-4, 0, 0);
    setupMatrices(shader, m_modelMatrix);
    shader->setUniformValue( "kd", QVector3D(0.0, 1.0, 0.0) );
    m_cube->render();

    // Torus
    m_modelMatrix.translate(8, 0, 0);
    setupMatrices(shader, m_modelMatrix);
    shader->setUniformValue( "kd", QVector3D(0.0, 0.0, 1.0) );
    m_torus->render();
}
