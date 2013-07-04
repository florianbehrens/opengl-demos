#ifndef BASICUSAGESCENE_H
#define BASICUSAGESCENE_H

#include "abstractscene.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMutex>
#include "qopenglversionfunctions.h"
#include "qopenglvertexarrayobject.h"

class BasicUsageScene : public AbstractScene
{
public:
    BasicUsageScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareShaderProgram();
    void prepareVertexBuffers();

    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLBuffer *m_vertexPositionBuffer;
    QOpenGLBuffer *m_vertexColorBuffer;
    QOpenGLVertexArrayObject *m_vao;

    QMatrix4x4 m_modelMatrix;

    QMutex m_mutex;
};

#endif // BASICUSAGESCENE_H
