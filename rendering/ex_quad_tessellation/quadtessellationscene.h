#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "abstractscene.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglvertexarrayobject.h"
#include <QMatrix4x4>

class QOpenGLFunctions_4_0_Core;

class QuadTessellationScene : public AbstractScene
{
public:
    QuadTessellationScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewportMatrix;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_quadBuffer;
    MaterialPtr m_material;

    float m_theta;

    QOpenGLFunctions_4_0_Core* m_funcs;
    float m_tessLevel;
};

#endif // BASICSCENE_H
