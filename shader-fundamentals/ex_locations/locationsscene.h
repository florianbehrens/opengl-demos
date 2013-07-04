#ifndef LOCATIONSSCENE_H
#define LOCATIONSSCENE_H

#include "abstractscene.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglversionfunctions.h"
#include "qopenglvertexarrayobject.h"

class LocationsScene : public AbstractScene
{
public:
    LocationsScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareShaderProgram();
    void prepareVertexBuffers();

    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vertexPositionBuffer;
    QOpenGLBuffer m_vertexColorBuffer;
    QOpenGLVertexArrayObject m_vao;
};

#endif // LOCATIONSSCENE_H
