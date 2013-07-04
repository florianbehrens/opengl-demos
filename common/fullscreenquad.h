#ifndef FULLSCREENQUAD_H
#define FULLSCREENQUAD_H

#include <QObject>

#include "material.h"

#include <QOpenGLBuffer>
#include "qopenglvertexarrayobject.h"

class FullScreenQuad : public QObject
{
    Q_OBJECT
public:
    explicit FullScreenQuad( QObject* parent = 0 );

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const;

    void create();

    void render();

private:
    void generateVertexData( QVector<float>& vertices,
                             QVector<float>& texCoords );
    void updateVertexArrayObject();

    MaterialPtr m_material;

    // QVertexBuffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_textureCoordBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // FULLSCREENQUAD_H
