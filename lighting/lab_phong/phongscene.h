#ifndef PHONGSCENE_H
#define PHONGSCENE_H

#include "abstractscene.h"
#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Mesh;

class PhongScene : public AbstractScene
{
public:
    PhongScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // PHONGSCENE_H
