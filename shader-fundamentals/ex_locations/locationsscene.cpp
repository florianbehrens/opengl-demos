#include "locationsscene.h"

#include <QObject>
#include <QOpenGLContext>

LocationsScene::LocationsScene()
    : m_program(),
      m_vertexPositionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexColorBuffer( QOpenGLBuffer::VertexBuffer )
{
}

void LocationsScene::initialise()
{
    // Create a vertex array object (VAO) - more on this later!
    m_vao.create();
    m_vao.bind();

    // Load, compile and link the shader program
    prepareShaderProgram();

    // Prepare our geometry and associate it with shader program inputs
    prepareVertexBuffers();
}

void LocationsScene::update( float t )
{
    Q_UNUSED( t );
}

void LocationsScene::render()
{
    // Clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Make our shader program current
    m_program.bind();

    // Draw the triangle! Woot!
    glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void LocationsScene::resize( int w, int h )
{
    // Adjust the OpenGL viewport (output rectangle) to fit our new window size
    glViewport( 0, 0, w, h );
}

void LocationsScene::prepareShaderProgram()
{
    // Load and compile the vertex shader
    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/locations.vert" ) )
        qCritical() << "Could not compile vertex shader. Log:" << m_program.log();

    // Load and compile the fragment shader
    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/locations.frag" ) )
        qCritical() << "Could not compile fragment shader. Log:" << m_program.log();

    // Link the shaders together into a complete shader program (pipeline)
    if ( !m_program.link() )
        qCritical() << "Could not link shader program. Log:" << m_program.log();
}

void LocationsScene::prepareVertexBuffers()
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
    m_vertexPositionBuffer.create();
    m_vertexPositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexPositionBuffer.bind();
    m_vertexPositionBuffer.allocate( positionData, 3 * 3 * sizeof( float ) );

    // The vertex colors
    m_vertexColorBuffer.create();
    m_vertexColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexColorBuffer.bind();
    m_vertexColorBuffer.allocate( colorData, 3 * 3 * sizeof( float ) );

    //
    // Associate our VBO's with input attribute variables in the vertex shader
    //

    // Make sure our shader is the currently active one
    m_program.bind();

    // Bind the position buffer, switch it "on", tell OpenGL what the data format is
    m_vertexPositionBuffer.bind();
    m_program.enableAttributeArray( 0 );
    m_program.setAttributeBuffer( 0, GL_FLOAT, 0, 3 );

    // Bind the color buffer, switch it "on", tell OpenGL what the data format is
    m_vertexColorBuffer.bind();
    m_program.enableAttributeArray( 1 );
    m_program.setAttributeBuffer( 1, GL_FLOAT, 0, 3 );
}
