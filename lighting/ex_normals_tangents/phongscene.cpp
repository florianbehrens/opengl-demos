#include "phongscene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"

#include <QObject>

#include <math.h>

PhongScene::PhongScene( const QString& fileName )
    : CameraScene(),
      m_fileName( fileName ),
      m_modelMatrix(),
      m_mesh( 0 ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();

    m_camera->setPosition( QVector3D( 5.0f, 0.0f, 5.0f ) );
    m_camera->setViewCenter( QVector3D( 3.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void PhongScene::initialise()
{
    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( true );
    loader.setTangentGenerationEnabled( true );
    loader.load( m_fileName );

    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/phong.vert",
                          ":/shaders/phong.frag" );

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    m_lineMaterial = MaterialPtr( new Material );
    m_lineMaterial->setShaders( ":/shaders/line.vert",
                                ":/shaders/line.frag" );

    m_mesh->computeNormalLinesBuffer( m_lineMaterial, 5 );
    m_mesh->computeTangentLinesBuffer( m_lineMaterial, 5 );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void PhongScene::update( float t )
{
    Q_UNUSED( t );

    m_theta += 0.2;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    // Get the maximum extent of the object so that we can scale it down to unit size
    float maxExtent = m_mesh->boundingVolume()->maxExtent();

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 1.0f / maxExtent );
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
}

void PhongScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    shader->bind();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 100.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();

    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;

    shader = m_lineMaterial->shader();
    shader->bind();
    shader->setUniformValue( "mvp", mvp );
    shader->setUniformValue( "color", QVector4D( 1.0f, 0.0f, 0.0f, 1.0f ) );

    m_mesh->renderNormalLines();

    shader->setUniformValue( "color", QVector4D( 0.0f, 1.0f, 0.0f, 1.0f ) );
    m_mesh->renderTangentLines();
}

