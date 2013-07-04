#ifndef TEXTURESCENE_H
#define TEXTURESCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Cube;

class TextureScene : public CameraScene
{
    Q_OBJECT

public:
    TextureScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

    void toggleCubeRotation()
    {
        m_rotateCube = !m_rotateCube;
    }

private:
    MaterialPtr createTextureMaterial( const QString& fileName );

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Cube* m_cube;
    float m_theta;
    bool m_rotateCube;
};

#endif // TEXTURESCENE_H
