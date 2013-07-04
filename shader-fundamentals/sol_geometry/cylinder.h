#ifndef CYLINDER_H
#define CYLINDER_H

#include <QObject>

#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>

class Cylinder : public QObject
{
    Q_OBJECT

    Q_PROPERTY( float radius READ radius WRITE setRadius )
    Q_PROPERTY( int rings READ rings WRITE setRings )
    Q_PROPERTY( int slices READ slices WRITE setSlices )

public:
    explicit Cylinder( QObject* parent = 0 );

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const;

    float radius() const
    {
        return m_radius;
    }

    float length() const
    {
        return m_length;
    }

    int rings() const
    {
        return m_rings;
    }

    int slices() const
    {
        return m_slices;
    }

    QOpenGLVertexArrayObject* vertexArrayObject() { return &m_vao; }

    int indexCount() const
    {
        return 6 * m_slices * m_rings // Sides
                + 2 * 3 * m_slices;   // Endcaps
    }

public slots:
    void setRadius( float arg )
    {
        m_radius = arg;
    }

    void setLength( float arg )
    {
        m_length = arg;
    }

    void setRings( int arg )
    {
        m_rings = arg;
    }

    void setSlices( int arg )
    {
        m_slices = arg;
    }

    void create();
    void render();

private:
    void generateVertexData( QVector<float>& vertices,
                             QVector<float>& normals,
                             QVector<unsigned int>& indices );
    void generateEndCapVertexData( float y,
                                   int& vertexIndex,
                                   int& elIndex,
                                   QVector<float>& vertices,
                                   QVector<float>& normals,
                                   QVector<unsigned int>& indices );
    void updateVertexArrayObject();

    MaterialPtr m_material;

    float m_radius;
    float m_length;
    int m_rings;  // Rings of latitude
    int m_slices; // Longitude

    // QVertexBuffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // CYLINDER_H
