#ifndef EARTHSCENE_H
#define EARTHSCENE_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Sphere;

class EarthScene : public AbstractScene
{
    Q_OBJECT

public:
    EarthScene( QObject* parent = 0 );

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
    MaterialPtr createMaterial( const QString& diffuseFileName,
                                const QString& specularFileName,
                                const QString& alphaFileName );

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Sphere* m_sphere;
    float m_theta;
    bool m_rotate;
};

#endif // EARTHSCENE_H
