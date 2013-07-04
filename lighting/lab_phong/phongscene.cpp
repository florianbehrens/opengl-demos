#include "phongscene.h"

#include "mesh.h"
#include "objloader.h"

#include <math.h>

PhongScene::PhongScene()
    : m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_mesh( 0 ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();
}

void PhongScene::initialise()
{
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( false );
    loader.setTangentGenerationEnabled( false );
    loader.load( "" );

    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/phong.vert",
                          ":/shaders/phong.frag"  );

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

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

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 0.2 );
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( -30.0f, 1.0f, 0.0f, 0.0f );
}

void PhongScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    shader->bind();

    // Set up the view matrix
    QVector3D eye( 1.0f, 0.2f, 0.3f );
    QVector3D center( 0.0f, 0.0f, 0.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );

    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();
}

void PhongScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 60.0f, aspect, 0.3f, 1000.0f );
}

