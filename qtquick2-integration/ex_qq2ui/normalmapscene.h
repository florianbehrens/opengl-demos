#ifndef TEXTURESCENE_H
#define TEXTURESCENE_H

#include <QObject>

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>
#include <QMutex>

class Camera;
class Mesh;

#if !defined(Q_OS_MAC)
class QOpenGLFunctions_3_3_Compatibility;
#else
class QOpenGLFunctions_2_1;
#endif

class NormalMapScene : public AbstractScene
{
    Q_OBJECT

    Q_PROPERTY( float rotationSpeed READ rotationSpeed WRITE setRotationSpeed NOTIFY rotationSpeedChanged )

public:
    NormalMapScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void cleanup();
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



    void setRotationSpeed( float rotationSpeed );
    float rotationSpeed() const;

public slots:
    void toggleRotation()
    {
        m_rotate = !m_rotate;
    }

signals:
    void rotationSpeedChanged();

private:
    MaterialPtr createTextureMaterial( const QString& diffuseFileName,
                                       const QString& normalFileName );

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    // Model related members
    QMatrix4x4 m_modelMatrix;
    Mesh* m_mesh;
    float m_theta;
    bool m_rotate;
    float m_rotationSpeed;

#if !defined(Q_OS_MAC)
    QOpenGLFunctions_3_3_Compatibility* m_funcs;
#else
    QOpenGLFunctions_2_1* m_funcs;
#endif
    QMutex m_mutex;
};

#endif // TEXTURESCENE_H
