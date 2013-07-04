#include "plane.h"

#include <QVector>

Plane::Plane( QObject* parent )
    : QObject( parent ),
      m_width( 1.0f ),
      m_height( 1.0f ),
      m_xDivisions( 1 ),
      m_yDivisions( 1 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

Plane::Plane( float width, float height, int xDivisions, int yDivisions, QObject* parent )
    : QObject( parent ),
      m_width( width ),
      m_height( height ),
      m_xDivisions( xDivisions ),
      m_yDivisions( yDivisions ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

void Plane::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr Plane::material() const
{
    return m_material;
}

void Plane::create()
{
    // Allocate some storage to hold per-vertex data
    QVector<float> v;         // Vertices
    QVector<float> n;         // Normals
    QVector<float> tex;       // Tex coords
    QVector<float> t;         // Tangents
    QVector<unsigned int> el; // Element indices

    // Generate the vertex data
    generateVertexData( v, n, tex, t, el );

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( float ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n.constData(), n.size() * sizeof( float ) );

    m_textureCoordBuffer.create();
    m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureCoordBuffer.bind();
    m_textureCoordBuffer.allocate( tex.constData(), tex.size() * sizeof( float ) );

    m_tangentBuffer.create();
    m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentBuffer.bind();
    m_tangentBuffer.allocate( t.constData(), t.size() * sizeof( float ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el.constData(), el.size() * sizeof( unsigned int ) );

    updateVertexArrayObject();
}

void Plane::generateVertexData( QVector<float>& vertices, QVector<float>& normals,
                                QVector<float>& texCoords, QVector<float>& tangents,
                                QVector<unsigned int>& indices )
{
    vertices.resize( 3 * ( m_xDivisions + 1 ) * ( m_yDivisions + 1 ) );
    normals.resize( 3 * ( m_xDivisions + 1 ) * ( m_yDivisions + 1 ) );
    texCoords.resize( 2 * ( m_xDivisions + 1 ) * ( m_yDivisions + 1 ) );
    tangents.resize( 3 * ( m_xDivisions + 1 ) * ( m_yDivisions + 1 ) );
    indices.resize( 6 * m_xDivisions * m_yDivisions );

    float x2 = m_width / 2.0f;
    float y2 = m_height / 2.0f;
    float iFactor = static_cast<float>( m_height ) / static_cast<float>( m_yDivisions );
    float jFactor = static_cast<float>( m_width ) / static_cast<float>( m_xDivisions );
    float texi = 1.0f / m_yDivisions;
    float texj = 1.0f / m_xDivisions;

    int vertexIndex = 0;
    int textureCoordIndex = 0;
    for ( int i = 0; i <= m_yDivisions; ++i )
    {
        float y = iFactor * i - y2;

        for ( int j = 0; j <= m_xDivisions; ++j )
        {
            float x = jFactor * j - x2;

            vertices[vertexIndex] = x;
            vertices[vertexIndex+1] = 0.0f;
            vertices[vertexIndex+2] = y;

            normals[vertexIndex] = 0.0f;
            normals[vertexIndex+1] = 1.0f;
            normals[vertexIndex+2] = 0.0f;

            tangents[vertexIndex] = 1.0f;
            tangents[vertexIndex+1] = 0.0f;
            tangents[vertexIndex+2] = 0.0f;

            vertexIndex += 3;

            texCoords[textureCoordIndex] = j * texi;
            texCoords[textureCoordIndex+1] = i * texj;

            textureCoordIndex += 2;
        }
    }

    int index = 0;
    for ( int i = 0; i < m_yDivisions; ++i )
    {
        unsigned int rowStart = i * ( m_xDivisions + 1 );
        unsigned int nextRowStart = ( i + 1 ) * ( m_xDivisions + 1 );

        for ( int j = 0; j < m_xDivisions; ++j )
        {
            indices[index]   = rowStart + j;
            indices[index+1] = nextRowStart + j;
            indices[index+2] = nextRowStart + j + 1;
            indices[index+3] = rowStart + j;
            indices[index+4] = nextRowStart + j + 1;
            indices[index+5] = rowStart + j + 1;
            index += 6;
        }
    }
}

void Plane::updateVertexArrayObject()
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

    m_normalBuffer.bind();
    shader->enableAttributeArray( "vertexNormal" );
    shader->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );

    m_textureCoordBuffer.bind();
    shader->enableAttributeArray( "vertexTexCoord" );
    shader->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );

    m_tangentBuffer.bind();
    shader->enableAttributeArray( "vertexTangent" );
    shader->setAttributeBuffer( "vertexTangent", GL_FLOAT, 0, 3 );

    m_indexBuffer.bind();

    // End VAO setup
    m_vao.release();
}

void Plane::render()
{
    // Bind the vertex array object to set up our vertex buffers and index buffer
    m_vao.bind();

    // Draw it!
    glDrawElements( GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0 );
}
