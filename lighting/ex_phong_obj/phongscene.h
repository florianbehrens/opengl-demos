#ifndef PHONGSCENE_H
#define PHONGSCENE_H

#include "camerascene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

class PhongScene : public CameraScene
{
public:
    PhongScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

private:
    QString m_fileName;

    QMatrix4x4 m_modelMatrix;

    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // PHONGSCENE_H
