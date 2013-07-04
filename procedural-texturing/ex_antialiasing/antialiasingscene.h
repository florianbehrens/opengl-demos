#ifndef ANTIALIASINGSCENE_H
#define ANTIALIASINGSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Plane;

class AntiAliasingScene : public CameraScene
{
    Q_OBJECT

public:
    AntiAliasingScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

    void toggleRotation()
    {
        m_rotate = !m_rotate;
    }

private:
    QMatrix4x4 m_modelMatrix;
    Plane* m_obj;
    float m_theta;
    bool m_rotate;

    // Time in seconds
    float m_time;
};

#endif // ANTIALIASINGSCENE_H
