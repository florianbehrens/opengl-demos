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
    indices.resize( 6 * faces + 2 * 3 * m_slices );

    // TODO Generate vertex positions and normals for the side of the cylinder

    // TODO If using indexed drawing generate indices to draw triangles

    // TODO: Optionally generate the vertex data and indices for the two end caps
    //generateEndCapVertexData( m_length / 2.0f, index, elIndex,
    //                          vertices, normals, indices );
    //generateEndCapVertexData( -m_length / 2.0f, index, elIndex,
    //                          vertices, normals, indices );
}

void Cylinder::generateEndCapVertexData( float y,
                                         int& index,
                                         int& elIndex,
                                         QVector<float>& vertices,
                                         QVector<float>& normals,
                                         QVector<unsigned int>& indices )
{
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

    // TODO Draw it!

    m_vao.release();
}
