#ifndef NOISESCENE_H
#define NOISESCENE_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Sphere;

class NoiseScene : public AbstractScene
{
    Q_OBJECT

public:
    NoiseScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    // Camera motion control
    void setSideSpeed( float vx ) { m_vx = vx; }
    void setVerticalSpeed( float vy ) { m_vy = vy; }
    void setForwardSpeed( float vz ) { m_vz = vz; }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }

    // Camera orientation control
    void pan( float angle ) { m_panAngle = angle; }
    void tilt( float angle ) { m_tiltAngle = angle; }

    void toggleRotation()
    {
        m_rotate = !m_rotate;
    }

private:
    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Sphere* m_obj;
    float m_theta;
    bool m_rotate;

    // Time in seconds
    float m_time;
};

#endif // NOISESCENE_H
