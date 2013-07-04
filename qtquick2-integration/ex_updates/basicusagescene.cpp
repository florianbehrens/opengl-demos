#include "basicusagescene.h"

#include <QObject>
#include <QOpenGLContext>

#include <cmath>

/*
static void dumpTextureUnitConfiguration()
{
    GLint textureUnit = 0;
    glGetIntegerv( GL_ACTIVE_TEXTURE, &textureUnit );
    textureUnit -= GL_TEXTURE0;
    qDebug() << "Active Texture Unit =" << textureUnit;

    GLint textureUnitCount;
    glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnitCount );
    qDebug() << "Found" << textureUnitCount << "available texture units";

    for ( int i = 0; i < textureUnitCount; ++i )
    {
        glActiveTexture( GL_TEXTURE0 + i );
        GLint textureId = 0;
        glGetIntegerv( GL_TEXTURE_BINDING_2D, &textureId );
        qDebug() << "Unit: <<" << i << "Texture Id =" << textureId;
    }

    glActiveTexture( GL_TEXTURE0 + textureUnit );
}
*/

BasicUsageScene::BasicUsageScene()
    : m_shaderProgram( 0 ),
      m_vertexPositionBuffer( 0 ),
      m_vertexColorBuffer( 0 ),
      m_vao( 0 )
{   
}

void BasicUsageScene::initialise()
{
    //dumpTextureUnitConfiguration();

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
}

void BasicUsageScene::update( float t )
{
    QMutexLocker locker( &m_mutex );
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( 25.0 * t, 0.0f, 0.0f, 1.0f );
}

void BasicUsageScene::render()
{
    //dumpTextureUnitConfiguration();

    //qDebug() << Q_FUNC_INFO;

    // Clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Make our shader program current
    m_shaderProgram->bind();

    // Set uniform variable
    {
        QMutexLocker locker( &m_mutex );
        m_shaderProgram->setUniformValue( "modelMatrix", m_modelMatrix );
    }

    // Bind the VAO
    m_vao->bind();

#if defined(Q_OS_MAC)
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    // Bind the position buffer, switch it "on", tell OpenGL what the data format is
    m_vertexPositionBuffer->bind();
    m_shaderProgram->enableAttributeArray( "vertexPosition" );
    m_shaderProgram->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    // Bind the color buffer, switch it "on", tell OpenGL what the data format is
    m_vertexColorBuffer->bind();
    m_shaderProgram->enableAttributeArray( "vertexColor" );
    m_shaderProgram->setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
#endif

    // Draw the triangle! Woot!
    glDrawArrays( GL_TRIANGLES, 0, 3 );

    // Unbind the VAO
    m_vao->release();

#if defined(Q_OS_MAC)
    glPopClientAttrib();
#endif

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
#if defined(Q_OS_MAC)
    // Load and compile the vertex shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/basic_2_1.vert" ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shaderProgram->log();

    // Load and compile the fragment shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/basic_2_1.frag" ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shaderProgram->log();
#else
    // Load and compile the vertex shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Vertex, ":/shaders/basic.vert" ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shaderProgram->log();

    // Load and compile the fragment shader
    if ( !m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Fragment, ":/shaders/basic.frag" ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shaderProgram->log();
#endif

    // Link the shaders together into a complete shader program (pipeline)
    if ( !m_shaderProgram->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shaderProgram->log();
}

void BasicUsageScene::prepareVertexBuffers()
{
    // The data for our triangle
    const float r = 0.8f;
    const float pi = 4.0f * atan( 1.0f );
    float positionData[] = {
        r * cos( 7.0f * pi / 6.0f ), r * sin( 7.0f * pi / 6.0f ), 0.0f,
        r * cos( 11.0f * pi / 6.0f ), r * sin( 11.0f * pi / 6.0f ), 0.0f,
        0.0f, r, 0.0f };
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
