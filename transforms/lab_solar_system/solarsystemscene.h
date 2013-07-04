#ifndef SolarSystemScene_H
#define SolarSystemScene_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Sphere;

class OrbitingBody;

class SolarSystemScene : public AbstractScene
{
    Q_OBJECT

public:
    SolarSystemScene( QObject* parent = 0 );

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

    void toggleAnimation()
    {
        m_animate = !m_animate;
    }

private:
    void createOrbitingBodies();
    void setBodyColor( const QOpenGLShaderProgramPtr& shader,
                       OrbitingBody* body ) const;

    MaterialPtr createSunMaterial();
    MaterialPtr createPhongMaterial();

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    QMatrix4x4 m_modelMatrix;
    Sphere* m_sphere;
    MaterialPtr m_sunMaterial;
    bool m_animate;
    float m_time;

    QVector<OrbitingBody*> m_bodies;
};

#endif // SolarSystemScene_H
