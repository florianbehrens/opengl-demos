#ifndef SPECULARMAPSCENE_H
#define SPECULARMAPSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Cube;

class SpecularMapScene : public CameraScene
{
    Q_OBJECT

public:
    SpecularMapScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

    void toggleCubeRotation()
    {
        m_rotateCube = !m_rotateCube;
    }

    void toggleMultiSampling();

private:
    MaterialPtr createTextureMaterial( const QString& diffuseFileName,
                                       const QString& specularFileName );


    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Cube* m_cube;
    float m_theta;
    bool m_rotateCube;
    bool m_multisample;
};

#endif // SPECULARMAPSCENE_H
