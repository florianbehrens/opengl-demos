#include "diffusescene.h"

#include "torus.h"

#include <QObject>

#include <math.h>

DiffuseScene::DiffuseScene()
    : m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_program( new QOpenGLShaderProgram ),
      m_torus( 0 ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();
}

void DiffuseScene::initialise()
{
    // Create a material
    prepareShaderProgram( ":/shaders/diffuse.vert", ":/shaders/diffuse.frag" );
    MaterialPtr material( new Material );
    material->setShader( m_program );

    // Create a torus (hmmm doughnuts)
    m_torus = new Torus;
    m_torus->setMinorRadius( 0.5f );
    m_torus->setMaterial( material );
    m_torus->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void DiffuseScene::update( float t )
{
    Q_UNUSED( t );

    m_theta += 0.25;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 0.2f );
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( -30.0f, 1.0f, 0.0f, 0.0f );
}

void DiffuseScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_program->bind();

    // Set up the view matrix
    QVector3D eye( 1.0f, 0.2f, 0.3f );
    QVector3D center( 0.0f, 0.0f, 0.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );

    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;
    m_program->setUniformValue( "modelViewMatrix", modelViewMatrix );
    m_program->setUniformValue( "normalMatrix", normalMatrix );
    m_program->setUniformValue( "projectionMatrix", m_projectionMatrix );
    m_program->setUniformValue( "mvp", mvp );

    // Set the lighting parameters
    m_program->setUniformValue( "lightPosition", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    m_program->setUniformValue( "lightIntensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the diffuse reflectivity of the object
    m_program->setUniformValue( "kd", QVector3D( 0.9f, 0.5f, 0.3f ) );

    // Let the mesh setup the remainder of its state and draw itself
    m_torus->render();
}

void DiffuseScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 60.0f, aspect, 0.3f, 1000.0f );
}

void DiffuseScene::prepareShaderProgram( const QString& vertexShader,
                                         const QString& fragmentShader )
{
    // Load and compile the vertex shader
    if ( !m_program->addShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << "Could not compile vertex shader. Log:" << m_program->log();

    // Load and compile the fragment shader
    if ( !m_program->addShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << "Could not compile fragment shader. Log:" << m_program->log();

    // Link the shaders together into a complete shader program (pipeline)
    if ( !m_program->link() )
        qCritical() << "Could not link shader program. Log:" << m_program->log();
}
