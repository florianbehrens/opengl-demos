#include "fullscreenquad.h"

FullScreenQuad::FullScreenQuad(QObject *parent)
    : QObject( parent ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vao()
{
}

void FullScreenQuad::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr FullScreenQuad::material() const
{
    return m_material;
}

void FullScreenQuad::create()
{
    // Allocate some storage to hold per-vertex data
    QVector<float> v;         // Vertices
    QVector<float> tex;       // Tex coords

    // Generate the vertex data
    generateVertexData( v, tex );

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( float ) );

    m_textureCoordBuffer.create();
    m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureCoordBuffer.bind();
    m_textureCoordBuffer.allocate( tex.constData(), tex.size() * sizeof( float ) );

    updateVertexArrayObject();
}

void FullScreenQuad::generateVertexData( QVector<float>& vertices,
                                         QVector<float>& texCoords )
{
    // Array for full-screen quad
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tex[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    vertices.resize( 18 );
    texCoords.resize( 12 );

    memcpy( vertices.data(), verts, 18 * sizeof( float ) );
    memcpy( texCoords.data(), tex, 12 * sizeof( float ) );
}

void FullScreenQuad::updateVertexArrayObject()
{
    // Ensure that we have a valid material and geometry
    if ( !m_material || !m_positionBuffer.isCreated() )
        return;

    // Create a vertex array object
    if ( !m_vao.isCreated() )
        m_vao.create();
    m_vao.bind();

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_textureCoordBuffer.bind();
    shader->enableAttributeArray( "vertexTexCoord" );
    shader->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );

    // End VAO setup
    m_vao.release();
}

void FullScreenQuad::render()
{
    // Bind the vertex array object to set up our vertex buffers
    m_vao.bind();

    // Draw it!
    glDrawArrays( GL_TRIANGLES, 0, 6 );
}
