#ifndef MULTIPLELIGHTSSCENE_H
#define MULTIPLELIGHTSSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

struct Light
{
    QVector4D position;
    QVector3D color;
};

class MultipleLightsScene : public AbstractScene
{
public:
    MultipleLightsScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareLights();

    QString m_fileName;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;
    QVector<Light> m_lights;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // MULTIPLELIGHTSSCENE_H
