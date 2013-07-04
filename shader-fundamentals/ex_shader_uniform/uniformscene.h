#ifndef UNIFORMSCENE_H
#define UNIFORMSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class UniformScene : public AbstractScene
{
public:
    UniformScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareShaderProgram();
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vertexPositionBuffer;
    QOpenGLBuffer m_vertexColorBuffer;
    QOpenGLVertexArrayObject m_vao;

    float m_theta;
    QMatrix4x4 m_rotationMatrix;
};

#endif // UNIFORMSCENE_H
