#include "bumpscene.h"

#include <camera.h>
#include <sphere.h>

#include <QGLWidget>
#include <QImage>

BumpScene::BumpScene( QObject* parent )
    : CameraScene( parent ),
      m_sphere( 0 ),
      m_theta( 0.0f ),
      m_rotate( false ),
      m_drawNormals( false )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 6.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void BumpScene::initialise()
{
    MaterialPtr material = createMaterial();

    // Create a cube and set the material on it
    m_sphere = new Sphere( this );
    m_sphere->setMaterial( material );
    m_sphere->setSlices( 32 );
    m_sphere->setRings( 16 );
    m_sphere->create();

    m_lineMaterial = MaterialPtr( new Material );
    m_lineMaterial->setShaders( ":/shaders/line.vert",
                                ":/shaders/line.frag" );

    m_sphere->computeTangentLinesBuffer( m_lineMaterial, 0.08 );
    m_sphere->computeNormalLinesBuffer( m_lineMaterial, 0.08 );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void BumpScene::update( float t )
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
}

void BumpScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_sphere->material()->bind();
    QOpenGLShaderProgramPtr shader = m_sphere->material()->shader();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters - specify the direction in world coordinates
    // and transform to eye space as the shader expects
    QVector4D worldLightDirection( 4.0f, 2.0f, -6.0f, 0.0f );
    QVector4D eyeLightDirection = m_camera->viewMatrix() * worldLightDirection;
    shader->setUniformValue( "light.position", eyeLightDirection );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );

    // Let the mesh setup the remainder of its state and draw itself
    m_sphere->render();

    if ( m_drawNormals )
    {
        QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;

        shader = m_lineMaterial->shader();
        shader->bind();
        shader->setUniformValue( "mvp", mvp );
        shader->setUniformValue( "color", QVector4D( 1.0f, 0.0f, 0.0f, 1.0f ) );

        m_sphere->renderNormalLines();

        shader->setUniformValue( "color", QVector4D( 0.0f, 1.0f, 0.0f, 1.0f ) );
        m_sphere->renderTangentLines();
    }
}

MaterialPtr BumpScene::createMaterial()
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/bumpmap.vert",
                          ":/shaders/bumpmap.frag" );
    return material;
}
