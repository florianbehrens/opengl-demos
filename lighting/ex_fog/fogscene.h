#ifndef FOGSCENE_H
#define FOGSCENE_H

#include "camerascene.h"
#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Torus;

class FogScene : public CameraScene
{
public:
    FogScene();

    virtual void initialise();
    virtual void render();
    virtual void update(float t);

private:

    QMatrix4x4 m_modelMatrix;
    Torus* m_torus;
};

#endif // FOGSCENE_H
