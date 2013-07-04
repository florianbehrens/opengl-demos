#ifndef BUMPSCENE_H
#define BUMPSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Sphere;

class BumpScene : public CameraScene
{
    Q_OBJECT

public:
    BumpScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

    void toggleRotation()
    {
        m_rotate = !m_rotate;
    }


    void toggleNormals()
    {
        m_drawNormals = !m_drawNormals;
    }

private:
    MaterialPtr createMaterial( );


    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Sphere* m_sphere;
    float m_theta;
    bool m_rotate;
    bool m_drawNormals;

    MaterialPtr m_lineMaterial;
};

#endif // BUMPSCENE_H
