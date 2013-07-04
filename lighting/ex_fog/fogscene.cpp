#include "fogscene.h"

#include "torus.h"

#include <math.h>

FogScene::FogScene()
    : CameraScene(),
      m_modelMatrix(),
      m_torus( 0 )
{
    m_modelMatrix.setToIdentity();

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 5.0f, 0.0f, 5.0f ) );
    m_camera->setViewCenter( QVector3D( 3.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void FogScene::initialise()
{
    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/fog.vert", ":/shaders/fog.frag" );

    // Create a torus (hmmm doughnuts)
    m_torus = new Torus;
    m_torus->setRings( 30 );
    m_torus->setSides( 25 );
    m_torus->setMinorRadius( 0.5f );
    m_torus->setMaterial( material );
    m_torus->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void FogScene::update(float t)
{
    Q_UNUSED(t);
}

void FogScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_torus->material()->shader();
    shader->bind();

    // Set the fog parameters
    shader->setUniformValue( "fog.color", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "fog.minDistance", 10.0f );
    shader->setUniformValue( "fog.maxDistance", 50.0f );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 0.8f, 0.8f, 0.8f ) );

    // Set the material properties
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.5f, 0.5f, 0.5f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    // Draw torus
    for ( int i = 0; i < 10; ++i )
    {
        // Calculate needed matrices
        m_modelMatrix.setToIdentity();
        m_modelMatrix.translate( 0.0f, 0.0f, -4.0f * float( i ) );

        m_camera->setStandardUniforms( shader, m_modelMatrix );

        // Draw torus
        m_torus->render();
    }
}

