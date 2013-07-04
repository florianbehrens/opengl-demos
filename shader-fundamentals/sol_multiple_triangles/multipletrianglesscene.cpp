#include "multipletrianglesscene.h"

#include <QObject>
#include <QOpenGLContext>
#include <QTime>

MultipleTrianglesScene::MultipleTrianglesScene()
    : m_program(),
      m_vertexPositionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexColorBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexCount( 0 )
{
}

void MultipleTrianglesScene::initialise()
{
    // Create a vertex array object (VAO) - more on this later!
    m_vao.create();
    m_vao.bind();

    // Load, compile and link the shader program
    prepareShaderProgram();

    // Prepare our geometry and associate it with shader program inputs
    //prepareVertexBuffers2Triangles();
    prepareVertexBuffersLoadsOfTriangles();

    m_time.start();
}

void MultipleTrianglesScene::update( float t )
{
    Q_UNUSED( t );
}

void MultipleTrianglesScene::render()
{
    // Output time since previous frame
    float t = float( m_time.elapsed() ) / 1000.0f;
    m_time.restart();
    qDebug() << "delta t =" << t;

    // Clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Make our shader program current
    m_program.bind();

    // Draw the triangle! Woot!
    glDrawArrays( GL_TRIANGLES, 0, m_vertexCount );
}

void MultipleTrianglesScene::resize( int w, int h )
{
    // Adjust the OpenGL viewport (output rectangle) to fit our new window size
    glViewport( 0, 0, w, h );
}

void MultipleTrianglesScene::prepareShaderProgram()
{
    // Load and compile the vertex shader
    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/basic.vert" ) )
        qCritical() << "Could not compile vertex shader. Log:" << m_program.log();

    // Load and compile the fragment shader
    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/basic.frag" ) )
        qCritical() << "Could not compile fragment shader. Log:" << m_program.log();

    // Link the shaders together into a complete shader program (pipeline)
    if ( !m_program.link() )
        qCritical() << "Could not link shader program. Log:" << m_program.log();
}

void MultipleTrianglesScene::prepareVertexBuffers2Triangles()
{
    // Two triangles => 6 vertices
    m_vertexCount = 6;

    // The data for our triangles
    float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.0f,  0.8f, 0.0f,
        -0.8f,  0.8f, 0.5f,
         0.8f,  0.8f, 0.5f,
         0.0f, -0.8f, 0.5f
    };
    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    //
    // Populate the buffer objects
    //

    // The vertex positions
    m_vertexPositionBuffer.create();
    m_vertexPositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexPositionBuffer.bind();
    m_vertexPositionBuffer.allocate( positionData, m_vertexCount * 3 * sizeof( float ) );

    // The vertex colors
    m_vertexColorBuffer.create();
    m_vertexColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexColorBuffer.bind();
    m_vertexColorBuffer.allocate( colorData, m_vertexCount * 3 * sizeof( float ) );

    //
    // Associate our VBO's with input attribute variables in the vertex shader
    //

    // Make sure our shader is the currently active one
    m_program.bind();

    // Bind the position buffer, switch it "on", tell OpenGL what the data format is
    m_vertexPositionBuffer.bind();
    m_program.enableAttributeArray( "vertexPosition" );
    m_program.setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    // Bind the color buffer, switch it "on", tell OpenGL what the data format is
    m_vertexColorBuffer.bind();
    m_program.enableAttributeArray( "vertexColor" );
    m_program.setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
}

void MultipleTrianglesScene::prepareVertexBuffersLoadsOfTriangles()
{
    // The data for our triangle
    qsrand( 0 );

    const int triangleCount = 200;
    m_vertexCount = 3 * triangleCount;
    QVector<float> positionData( 3 * m_vertexCount );
    QVector<float> colorData( 3 * m_vertexCount );

    float z = 0.0f;
    float r = 1.0f, g = 1.0f, b = 1.0f;
    for ( int i = 0; i < 3 * m_vertexCount; i += 3 )
    {
        // Generate a random triangle with vertices in the range [-1,1], [-1,1], [-1,1]
        float x = -1.0f + 2.0f * float( qrand() ) / float( RAND_MAX );
        float y = -1.0f + 2.0f * float( qrand() ) / float( RAND_MAX );
        if ( i % 9 == 0 )
            z = -1.0f + 2.0f * float( qrand() ) / float( RAND_MAX );

        positionData[i]   = x;
        positionData[i+1] = y;
        positionData[i+2] = z;

        if ( i % 9 == 0 )
        {
            r = 0.5 + 0.5 * float( qrand() ) / float( RAND_MAX );
            g = 0.5 + 0.5 * float( qrand() ) / float( RAND_MAX );
            b = 0.5 + 0.5 * float( qrand() ) / float( RAND_MAX );
        }
        colorData[i]   = r;
        colorData[i+1] = g;
        colorData[i+2] = b;
    }

    //
    // Populate the buffer objects
    //

    // The vertex positions
    m_vertexPositionBuffer.create();
    m_vertexPositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexPositionBuffer.bind();
    m_vertexPositionBuffer.allocate( positionData.data(), m_vertexCount * 3 * sizeof( float ) );

    // The vertex colors
    m_vertexColorBuffer.create();
    m_vertexColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexColorBuffer.bind();
    m_vertexColorBuffer.allocate( colorData.data(), m_vertexCount * 3 * sizeof( float ) );

    //
    // Associate our VBO's with input attribute variables in the vertex shader
    //

    // Make sure our shader is the currently active one
    m_program.bind();

    // Bind the position buffer, switch it "on", tell OpenGL what the data format is
    m_vertexPositionBuffer.bind();
    m_program.enableAttributeArray( "vertexPosition" );
    m_program.setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    // Bind the color buffer, switch it "on", tell OpenGL what the data format is
    m_vertexColorBuffer.bind();
    m_program.enableAttributeArray( "vertexColor" );
    m_program.setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
}
