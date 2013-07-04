#ifndef NORMALMAPSCENE_H
#define NORMALMAPSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Cube;
class Plane;

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
        m_rotateCube = !m_rotateCube;
    }

private:
    MaterialPtr createTextureMaterial( const QString& diffuseFileName,
                                       const QString& specularFileName,
                                       const QString& normalFileName );

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Cube* m_obj;
    float m_theta;
    bool m_rotateCube;
};

#endif // NORMALMAPSCENE_H
