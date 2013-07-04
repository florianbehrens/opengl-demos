#ifndef WIREFRAMESCENE_H
#define WIREFRAMESCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Camera;
class Mesh;

class WireframeScene : public AbstractScene
{
    Q_OBJECT

public:
    WireframeScene( const QString& fileName, QObject* parent = 0 );

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

    void toggleUpdates()
    {
        m_updatesEnabled = !m_updatesEnabled;
    }

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    QString m_fileName;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewportMatrix;

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    Mesh* m_mesh;

    // Rotation of object
    float m_theta;

    float m_time;

    bool m_updatesEnabled;
};

#endif // WIREFRAMESCENE_H
