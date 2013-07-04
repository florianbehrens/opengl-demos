#include "grid.h"

#include <QVector>

Grid::Grid( QObject* parent )
    : QObject( parent ),
      m_width( 1.0f ),
      m_height( 1.0f ),
      m_xDivisions( 1 ),
      m_yDivisions( 1 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

Grid::Grid( float width, float height, int xDivisions, int yDivisions, QObject* parent )
    : QObject( parent ),
      m_width( width ),
      m_height( height ),
      m_xDivisions( xDivisions ),
      m_yDivisions( yDivisions ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

void Grid::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr Grid::material() const
{
    return m_material;
}

void Grid::create()
{
    // Allocate some storage to hold per-vertex data
    QVector<float> v;         // Vertices
    QVector<unsigned int> el; // Element indices

    // Generate the vertex data
    generateVertexData( v, el );

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( float ) );

#if 0
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el.constData(), el.size() * sizeof( unsigned int ) );
#endif
    updateVertexArrayObject();
}

void Grid::generateVertexData( QVector<float>& vertices,
                               QVector<unsigned int>& indices )
{
    Q_UNUSED( indices );
    vertices.resize( vertexCount() * 3);
    //indices.resize( 6 * m_xDivisions * m_yDivisions );

    float x2 = m_width / 2.0f;
    float y2 = m_height / 2.0f;
    float iFactor = static_cast<float>( m_height ) / static_cast<float>( m_yDivisions );
    float jFactor = static_cast<float>( m_width ) / static_cast<float>( m_xDivisions );
    int vertexIndex = 0;


    for ( int i = 0; i <= m_yDivisions; ++i )
    {
        float y = iFactor * i - y2;
        vertices[vertexIndex] = -x2;
        vertices[vertexIndex+1] = 0.0f;
        vertices[vertexIndex+2] = y;
        
        vertices[vertexIndex+3] = x2;
        vertices[vertexIndex+4] = 0.0f;
        vertices[vertexIndex+5] = y;
        
        vertexIndex += 6;
    }
    
    for ( int j = 0; j <= m_xDivisions; ++j )
    {
        float x = jFactor * j - x2;
        vertices[vertexIndex] = x;
        vertices[vertexIndex+1] = 0.0f;
        vertices[vertexIndex+2] = -y2;
        
        vertices[vertexIndex+3] = x;
        vertices[vertexIndex+4] = 0.0f;
        vertices[vertexIndex+5] = y2;
        
        vertexIndex += 6;
    }
}

void Grid::updateVertexArrayObject()
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

    //m_indexBuffer.bind();

    // End VAO setup
    m_vao.release();
}

int Grid::vertexCount() const
{
    return (2 * ( m_xDivisions + 1 )) + (2 * ( m_yDivisions + 1 ));
}

void Grid::render()
{
    // Bind the vertex array object to set up our vertex buffers and index buffer
    m_vao.bind();

    glDrawArrays(GL_LINES, 0, vertexCount());

    m_vao.release();
}
