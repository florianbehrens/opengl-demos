#ifndef MULTIPLETRIANGLESSCENE_H
#define MULTIPLETRIANGLESSCENE_H

#include "abstractscene.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglversionfunctions.h"
#include "qopenglvertexarrayobject.h"

#include <QTime>

class MultipleTrianglesScene : public AbstractScene
{
public:
    MultipleTrianglesScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareShaderProgram();
    void prepareVertexBuffers2Triangles();
    void prepareVertexBuffersLoadsOfTriangles();

    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vertexPositionBuffer;
    QOpenGLBuffer m_vertexColorBuffer;
    QOpenGLVertexArrayObject m_vao;

    QTime m_time;
    int m_vertexCount;
};

#endif // MULTIPLETRIANGLESSCENE_H
