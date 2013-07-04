#ifndef NORMALMAPSCENE_H
#define NORMALMAPSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Mesh;

class NormalMapScene : public CameraScene
{
    Q_OBJECT

public:
    NormalMapScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();


    void toggleRotation()
    {
        m_rotate = !m_rotate;
    }

private:
    MaterialPtr createTextureMaterial( const QString& diffuseFileName,
                                       const QString& normalFileName );

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Mesh* m_mesh;
    float m_theta;
    bool m_rotate;
};

#endif // NORMALMAPSCENE_H
