#ifndef UVAMPSCENE_H
#define UVAMPSCENE_H

#include "camerascene.h"
#include "material.h"
#include "mesh.h"

#include <QObject>

#include "qopenglvertexarrayobject.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class QPropertyAnimation;

class UVmapScene : public CameraScene
{
    Q_OBJECT


public:
    UVmapScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

    void toggleAnimation();
private:

    MaterialPtr createTextureMaterial( const QString& diffuseFileName );

    QMatrix4x4 m_modelMatrix;
    float m_theta;
    bool m_rotate;

    MaterialPtr m_material;
    Mesh* m_mesh;

    QOpenGLFunctions* m_funcs;
};

#endif // UVAMPSCENE_H
