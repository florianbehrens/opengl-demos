#include "mesh.h"

#include "axisalignedboundingbox.h"
#include "objloader.h"

Mesh::Mesh( QObject* parent )
    : QObject( parent ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao(),
      m_indexCount( 0 ),
      m_material(),
      m_points(),
      m_boundingVolume( new AxisAlignedBoundingBox() )
{
}

Mesh::~Mesh()
{

}

void Mesh::setMeshData( const ObjLoader& loader )
{
    // Construct buffers for the vertices, normals, texCoord, tangents and indices
    m_points = loader.vertices();
    QVector<QVector3D> normals = loader.normals();
    QVector<QVector2D> texCoords = loader.textureCoordinates();
    QVector<QVector4D> tangents = loader.tangents();

    int vertexCount  = m_points.size();
    QVector<unsigned int> elements = loader.indices();
    m_indexCount = elements.size();
    int faces = elements.size() / 3;

    /** \todo Use QScopedPointers or try reinterpret_cast on the
      * QVector<> objects.
      */
    float* v = new float[3 * vertexCount];
    float* n = new float[3 * vertexCount];
    float* tc = 0;
    float* tang = 0;

    if ( texCoords.size() > 0 )
    {
        tc = new float[ 2 * vertexCount ];
        if ( !tangents.isEmpty() )
            tang = new float[ 4 * vertexCount ];
    }

    unsigned int* el = new unsigned int[elements.size()];

    int idx = 0, tcIdx = 0, tangIdx = 0;
    for ( int i = 0; i < vertexCount; ++i )
    {
        v[idx]   = m_points[i].x();
        v[idx+1] = m_points[i].y();
        v[idx+2] = m_points[i].z();
        n[idx]   = normals[i].x();
        n[idx+1] = normals[i].y();
        n[idx+2] = normals[i].z();
        idx += 3;
        if ( tc != 0 )
        {
            tc[tcIdx]   = texCoords[i].x();
            tc[tcIdx+1] = texCoords[i].y();
            tcIdx += 2;
        }
        if ( tang != 0 )
        {
            tang[tangIdx]   = tangents[i].x();
            tang[tangIdx+1] = tangents[i].y();
            tang[tangIdx+2] = tangents[i].z();
            tang[tangIdx+3] = tangents[i].w();
            tangIdx += 4;
        }
    }

    for ( int i = 0; i < elements.size(); ++i )
        el[i] = elements[i];

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v, 3 * vertexCount * sizeof( float ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n, 3 * vertexCount * sizeof( float ) );

    if ( tc )
    {
        m_textureCoordBuffer.create();
        m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_textureCoordBuffer.bind();
        m_textureCoordBuffer.allocate( tc, 2 * vertexCount * sizeof( float ) );
    }

    if ( tang )
    {
        m_tangentBuffer.create();
        m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_tangentBuffer.bind();
        m_tangentBuffer.allocate( tang, 4 * vertexCount * sizeof( float ) );
    }

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el, 3 * faces * sizeof( unsigned int ) );

    // Calculate the bounding volume
    m_boundingVolume->update( m_points );

    // Clean up
    delete[] v;
    delete[] n;
    if ( tc != NULL )
        delete[] tc;
    if ( tang != NULL )
        delete[] tang;
    delete[] el;

    setupVertexArrayObject();
}

void Mesh::render()
{
    // Bind the vertex array oobject to set up our vertex buffers and index buffer
    m_vao.bind();

    // Draw it!
    glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0 );

    m_vao.release();
}

void Mesh::setupVertexArrayObject()
{
    m_vao.create();
    m_vao.bind();
    m_material->shader()->bind();

    bindBuffers();

    m_vao.release();

    m_indexBuffer.release();
    m_normalBuffer.release();
}

AxisAlignedBoundingBox* Mesh::boundingVolume() const
{
    return m_boundingVolume.data();
}

void Mesh::computeNormalLinesBuffer( const MaterialPtr& mat, double scale )
{
    int vertexCount  = m_points.size();
    float* v = new float[6 * vertexCount];
    float* vPtr = v;

    m_normalBuffer.bind();
    float* n = reinterpret_cast<float*>( m_normalBuffer.map( QOpenGLBuffer::ReadOnly ) );
    Q_ASSERT(n);
    for ( int vIndex = 0; vIndex < vertexCount; ++vIndex )
    {
        QVector3D pt = m_points[vIndex];
        *vPtr++ = pt.x();
        *vPtr++ = pt.y();
        *vPtr++ = pt.z();
        *vPtr++ = pt.x() + (*n++ * scale);
        *vPtr++ = pt.y() + (*n++ * scale);
        *vPtr++ = pt.z() + (*n++ * scale);
    }

    m_normalBuffer.unmap();

    m_normalLines.create();
    m_normalLines.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalLines.bind();
    m_normalLines.allocate( v, 6 * vertexCount * sizeof( float ) );

    m_vaoNormals.create();
    m_vaoNormals.bind();
    mat->shader()->enableAttributeArray( "vertexPosition" );
    mat->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
    m_vaoNormals.release();
    m_normalLines.release();

    delete[] v;
}

void Mesh::computeTangentLinesBuffer( const MaterialPtr& mat, double scale )
{
    if (!m_tangentBuffer.isCreated()) {
        qWarning() << "computeTangentLinesBuffer: no tangents on mesh";
        return;
    }

    int nVerts = m_points.size();
    float* v = new float[6 * nVerts];
    float* vPtr = v;

    m_tangentBuffer.bind();
    float* t = reinterpret_cast<float*>( m_tangentBuffer.map( QOpenGLBuffer::ReadOnly ) );

    Q_ASSERT(t);
    for ( int vIndex = 0; vIndex < nVerts; ++vIndex )
    {
        QVector3D pt = m_points[vIndex];
        *vPtr++ = pt.x();
        *vPtr++ = pt.y();
        *vPtr++ = pt.z();
        *vPtr++ = pt.x() + (*t++ * scale);
        *vPtr++ = pt.y() + (*t++ * scale);
        *vPtr++ = pt.z() + (*t++ * scale);
        t++; // skip fourth tangent value
    }

    m_tangentBuffer.bind();
    m_tangentBuffer.unmap();

    m_tangentLines.create();
    m_tangentLines.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentLines.bind();
    m_tangentLines.allocate( v, 6 * nVerts * sizeof( float ) );

    m_vaoTangents.create();
    m_vaoTangents.bind();
    mat->shader()->enableAttributeArray( "vertexPosition" );
    mat->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
    m_vaoTangents.release();
    m_tangentLines.release();

    delete[] v;
}

void Mesh::renderNormalLines()
{
    m_vaoNormals.bind();
    glDrawArrays( GL_LINES, 0, m_points.size() * 2 );
    m_vaoNormals.release();
}

void Mesh::renderTangentLines()
{
    m_vaoTangents.bind();
    glDrawArrays( GL_LINES, 0, m_points.size() * 2 );
    m_vaoTangents.release();
}

void Mesh::bindBuffers()
{
    m_material->shader()->bind();

    m_positionBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexPosition" );
    m_material->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_normalBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexNormal" );
    m_material->shader()->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );

    if ( m_textureCoordBuffer.isCreated() )
    {
        m_textureCoordBuffer.bind();
        m_material->shader()->enableAttributeArray( "vertexTexCoord" );
        m_material->shader()->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );
    }

    if ( m_tangentBuffer.isCreated() )
    {
        m_tangentBuffer.bind();
        m_material->shader()->enableAttributeArray( "vertexTangent" );
        m_material->shader()->setAttributeBuffer( "vertexTangent", GL_FLOAT, 0, 4 );
    }

    m_indexBuffer.bind();
}
