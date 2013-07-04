#include "basicusagescene.h"

#include <QObject>
#include <QOpenGLContext>

BasicUsageScene::BasicUsageScene()
    : m_shaderProgram( 0 ),
      m_vertexPositionBuffer( 0 ),
      m_vertexColorBuffer( 0 ),
      m_vao( 0 )
{   
}

void BasicUsageScene::initialise()
{
    m_shaderProgram = new QOpenGLShaderProgram;
    m_vertexPositionBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
    m_vertexColorBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
    m_vao = new QOpenGLVertexArrayObject;

    // Create a vertex array object (VAO) - more on this later!
    m_vao->create();
    m_vao->bind();

    // Load, compile and link the shader program
    prepareShaderProgram();

    // Prepare our geometry and associate it with shader program inputs
    prepareVertexBuffers();

    m_vao->release();

    // Set the clear color
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void BasicUsageScene::cleanup()
{
    delete m_shaderProgram;
    delete m_vertexPositionBuffer;
    delete m_vertexColorBuffer;
    delete m_vao;
}

void BasicUsageScene::update( float t )
{
    Q_UNUSED( t );
}

void BasicUsageScene::render()
{
    //qDebug() << Q_FUNC_INFO;

    // Clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Make our shader program current
    m_shaderProgram->bind();

    // Bind the VAO
    m_vao->bind();

    // Draw the triangle! Woot!
    glDrawArrays( GL_TRIANGLES, 0, 3 );

    // Unbind the VAO
    m_vao->release();

    // Unbind the shader program
    m_shaderProgram->release();
}

void BasicUsageScene::resize( int w, int h )
{
    // Adjust the OpenGL viewport (output rectangle) to fit our new window size
    glViewport( 0, 0, w, h );
}

void BasicUsageScene::prepareShaderProgram()
{
    // Load and compile the vertex shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/basic.vert" ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shaderProgram->log();

    // Load and compile the fragment shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/basic.frag" ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shaderProgram->log();

    // Link the shaders together into a complete shader program (pipeline)
    if ( !m_shaderProgram->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shaderProgram->log();
}

void BasicUsageScene::prepareVertexBuffers()
{
    // The data for our triangle
    float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.0f,  0.8f, 0.0f
    };
    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    //
    // Populate the buffer objects
    //

    // The vertex positions
    m_vertexPositionBuffer->create();
    m_vertexPositionBuffer->setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexPositionBuffer->bind();
    m_vertexPositionBuffer->allocate( positionData, 3 * 3 * sizeof( float ) );

    // The vertex colors
    m_vertexColorBuffer->create();
    m_vertexColorBuffer->setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexColorBuffer->bind();
    m_vertexColorBuffer->allocate( colorData, 3 * 3 * sizeof( float ) );

    //
    // Associate our VBO's with input attribute variables in the vertex shader
    //

    // Make sure our shader is the currently active one
    m_shaderProgram->bind();

    // Bind the position buffer, switch it "on", tell OpenGL what the data format is
    m_vertexPositionBuffer->bind();
    m_shaderProgram->enableAttributeArray( "vertexPosition" );
    m_shaderProgram->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    // Bind the color buffer, switch it "on", tell OpenGL what the data format is
    m_vertexColorBuffer->bind();
    m_shaderProgram->enableAttributeArray( "vertexColor" );
    m_shaderProgram->setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );

    m_vertexPositionBuffer->release();
    m_vertexColorBuffer->release();
}
