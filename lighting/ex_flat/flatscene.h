#ifndef FLATSCENE_H
#define FLATSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

class FlatScene : public AbstractScene
{
public:
    FlatScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    QString m_fileName;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // FLATSCENE_H
