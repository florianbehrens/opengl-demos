#include "cylinder.h"

#include <QOpenGLShaderProgram>

#include <math.h>

const float pi = 3.14159265358979323846;
const float twoPi = 2.0 * pi;

Cylinder::Cylinder( QObject* parent )
    : QObject( parent ),
      m_radius( 1.0f ),
      m_length( 2.0f ),
      m_rings( 10 ),
      m_slices( 30 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

void Cylinder::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr Cylinder::material() const
{
    return m_material;
}

void Cylinder::create()
{
    // Allocate some storage to hold per-vertex data
    QVector<float> v;         // Vertices
    QVector<float> n;         // Normals
    QVector<unsigned int> el; // Element indices

    // Generate the vertex data
    generateVertexData( v, n, el );

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( float ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n.constData(), n.size() * sizeof( float ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el.constData(), el.size() * sizeof( unsigned int ) );

    updateVertexArrayObject();
}

void Cylinder::generateVertexData( QVector<float>& vertices,
                                   QVector<float>& normals,
                                   QVector<unsigned int>& indices  )
{
    int faces = m_slices * m_rings;           // Number of "rectangular" faces
    int nVerts  = ( m_slices + 1 ) * m_rings  // Sides
                + 2 * ( m_slices + 1 );       // 2 x endcaps

    // Resize vector to hold our data
    vertices.resize( 3 * nVerts );
    normals.resize( 3 * nVerts );
    indices.resize( ( m_rings - 1 ) * 6 * m_slices + 6 * m_slices );

    // The side of the cylinder
    const float dTheta = twoPi / static_cast<float>( m_slices );
    const float dy = m_length / static_cast<float>( m_rings - 1 );

    // Iterate over heights (rings)
    int index = 0;

    for ( int ring = 0; ring < m_rings; ++ring )
    {
        const float y = -m_length / 2.0f + static_cast<float>( ring ) * dy;

        // Iterate over slices (segments in a ring)
        for ( int slice = 0; slice < m_slices + 1; ++slice )
        {
            const float theta = static_cast<float>( slice ) * dTheta;
            const float cosTheta = cosf( theta );
            const float sinTheta = sinf( theta );

            vertices[index]   = m_radius * cosTheta;
            vertices[index+1] = y;
            vertices[index+2] = m_radius * sinTheta;

            normals[index]   = cosTheta;
            normals[index+1] = 0.0f;
            normals[index+2] = sinTheta;

            index += 3;
        }
    }

    int elIndex = 0;
    for ( int i = 0; i < m_rings - 1; ++i )
    {
        const int ringStartIndex = i * ( m_slices + 1 );
        const int nextRingStartIndex = ( i + 1 ) * ( m_slices + 1 );

        for ( int j = 0; j < m_slices; ++j )
        {
            // Split the quad into two triangles
            indices[elIndex]   = ringStartIndex + j;
            indices[elIndex+1] = ringStartIndex + j + 1;
            indices[elIndex+2] = nextRingStartIndex + j;
            indices[elIndex+3] = nextRingStartIndex + j;
            indices[elIndex+4] = ringStartIndex + j + 1;
            indices[elIndex+5] = nextRingStartIndex + j + 1;

            elIndex += 6;
        }
    }

    // Generate the vertex data and indices for the two end caps
    generateEndCapVertexData( m_length / 2.0f, index, elIndex,
                              vertices, normals, indices );
    generateEndCapVertexData( -m_length / 2.0f, index, elIndex,
                              vertices, normals, indices );
}

void Cylinder::generateEndCapVertexData( float y,
                                         int& index,
                                         int& elIndex,
                                         QVector<float>& vertices,
                                         QVector<float>& normals,
                                         QVector<unsigned int>& indices )
{
    // Make a note of the vertex index for the center of the end cap
    int endCapStartIndex = index / 3;

    vertices[index]   = 0.0f;
    vertices[index+1] = y;
    vertices[index+2] = 0.0f;

    normals[index]   = 0.0f;
    normals[index+1] = ( y >= 0.0f ) ? 1.0f : -1.0;
    normals[index+2] = 0.0f;

    index += 3;

    const float dTheta = twoPi / static_cast<float>( m_slices );
    for ( int slice = 0; slice < m_slices; ++slice )
    {
        const float theta = static_cast<float>( slice ) * dTheta;
        const float cosTheta = cosf( theta );
        const float sinTheta = sinf( theta );

        vertices[index]   = m_radius * cosTheta;
        vertices[index+1] = y;
        vertices[index+2] = m_radius * sinTheta;

        normals[index]   = 0.0f;
        normals[index+1] = ( y >= 0.0f ) ? 1.0f : -1.0;
        normals[index+2] = 0.0f;

        index += 3;
    }

    for ( int i = 0; i < m_slices; ++i )
    {
        // Use center point and i+1, and i+2 vertices to create triangles
        if ( i != m_slices - 1 )
        {
            indices[elIndex]   = endCapStartIndex;
            indices[elIndex+1] = endCapStartIndex + i + 1;
            indices[elIndex+2] = endCapStartIndex + i + 2;
        }
        else
        {
            indices[elIndex]   = endCapStartIndex;
            indices[elIndex+1] = endCapStartIndex + i + 1;
            indices[elIndex+2] = endCapStartIndex + 1;
        }

        elIndex += 3;
    }
}

void Cylinder::updateVertexArrayObject()
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

    m_indexBuffer.bind();

    // End VAO setup
    m_vao.release();

    // Tidy up after ourselves
    m_indexBuffer.release();
}

void Cylinder::render()
{
    // Bind the vertex array oobject to set up our vertex buffers and index buffer
    m_vao.bind();

    // Draw it!
    glDrawElements( GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0 );

    m_vao.release();
}
