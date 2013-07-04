#include "uniformscene.h"

#include <math.h>

UniformScene::UniformScene()
    : m_program(),
      m_vertexPositionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexColorBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vao(),
      m_theta( 0.0 ),
      m_rotationMatrix()
{
}

void UniformScene::initialise()
{
    prepareShaderProgram();
    prepareVertexBuffers();
    prepareVertexArrayObject();
}

void UniformScene::update( float t )
{
    Q_UNUSED( t );

    // Increment the angle a little
    m_theta += 0.5f;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    // Calculate a new rotation matrix
    m_rotationMatrix.setToIdentity();
    m_rotationMatrix.rotate( m_theta, 0.0f, 0.0f, 1.0f );
}

void UniformScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT );

    // Set current shader program and VAO
    m_vao.bind();
    m_program.bind();

    // Set the uniform variable on shader program
    m_program.setUniformValue( "rotationMatrix", m_rotationMatrix );

    // Draw the triangle
    glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void UniformScene::resize( int w, int h )
{
    glViewport( 0, 0, w, h );
}

void UniformScene::prepareShaderProgram()
{
    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/basic_uniform.vert" ) )
        qCritical() << "Could not compile vertex shader. Log:" << m_program.log();

    if ( !m_program.addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/basic_uniform.frag" ) )
        qCritical() << "Could not compile fragment shader. Log:" << m_program.log();

    if ( !m_program.link() )
        qCritical() << "Could not link shader program. Log:" << m_program.log();
}

void UniformScene::prepareVertexBuffers()
{
    const float r = 0.8f;
    const float pi = 4.0f * atan( 1.0f );
    float positionData[] = {
        r * cos( 7.0f * pi / 6.0f ), r * sin( 7.0f * pi / 6.0f ), 0.0f,
        r * cos( 11.0f * pi / 6.0f ), r * sin( 11.0f * pi / 6.0f ), 0.0f,
        0.0f, r, 0.0f };
    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f };

    // Populate the buffer objects
    m_vertexPositionBuffer.create();
    m_vertexPositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexPositionBuffer.bind();
    m_vertexPositionBuffer.allocate( positionData, 3 * 3 * sizeof( float ) );

    m_vertexColorBuffer.create();
    m_vertexColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexColorBuffer.bind();
    m_vertexColorBuffer.allocate( colorData, 3 * 3 * sizeof( float ) );
}

void UniformScene::prepareVertexArrayObject()
{
    // Create a VAO for this "object"
    m_vao.create();
    m_vao.bind();

    m_program.bind();
    m_vertexPositionBuffer.bind();
    m_program.enableAttributeArray( "vertexPosition" );
    m_program.setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_vertexColorBuffer.bind();
    m_program.enableAttributeArray( "vertexColor" );
    m_program.setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
}
