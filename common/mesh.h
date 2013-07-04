#ifndef MESH_H
#define MESH_H

#include <QObject>

#include "material.h"

#include <QScopedPointer>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglvertexarrayobject.h"

class AxisAlignedBoundingBox;
class ObjLoader;

class Mesh : public QObject
{
    Q_OBJECT
public:
    explicit Mesh( QObject* parent = 0 );
    virtual ~Mesh();

    void setMeshData( const ObjLoader& loader );

    void setMaterial( const MaterialPtr& material ) { m_material = material; }
    MaterialPtr material() const { return m_material; }

    AxisAlignedBoundingBox* boundingVolume() const;

    void bindBuffers();

    void render();

    /**
     * @brief computeNormalLinesBuffer - compute a vertex buffer suitable for
     * rendering with GL_LINES, showing each vertex normal
     */
    void computeNormalLinesBuffer( const MaterialPtr& mat, double scale = 1.0 );
    void computeTangentLinesBuffer( const MaterialPtr& mat, double scale = 1.0 );
    void renderNormalLines();
    void renderTangentLines();
private:
    void setupVertexArrayObject();

    // QGLBuffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureCoordBuffer;
    QOpenGLBuffer m_tangentBuffer;
    QOpenGLBuffer m_indexBuffer;

    // Vertex array object to quickly set render state
    QOpenGLVertexArrayObject m_vao;
    int m_indexCount;

    // The material
    MaterialPtr m_material;

    // Hold on to the vertex data itself to allow calculation of bounding volumes
    QVector<QVector3D> m_points;

    // The bounding volume
    QScopedPointer<AxisAlignedBoundingBox> m_boundingVolume;

    QOpenGLBuffer m_normalLines, m_tangentLines;
    QOpenGLVertexArrayObject m_vaoNormals, m_vaoTangents;
};

#endif // MESH_H
