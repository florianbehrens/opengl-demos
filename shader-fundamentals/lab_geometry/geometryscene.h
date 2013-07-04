#ifndef GEOMETRYSCENE_H
#define GEOMETRYSCENE_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class Camera;
class Cylinder;

class GeometryScene : public AbstractScene
{
    Q_OBJECT

public:
    GeometryScene( QObject* parent = 0 );

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
        m_rotateObject = !m_rotateObject;
    }

private:
    MaterialPtr createMaterial();

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    // Cube related members
    QMatrix4x4 m_modelMatrix;
    Cylinder* m_cylinder;
    float m_theta;
    bool m_rotateObject;
};

#endif // GeometryScene_H
