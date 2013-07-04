#ifndef TOONSCENE_H
#define TOONSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

class ToonScene : public AbstractScene
{
public:
    ToonScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    QString m_fileName;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // TOONSCENE_H
