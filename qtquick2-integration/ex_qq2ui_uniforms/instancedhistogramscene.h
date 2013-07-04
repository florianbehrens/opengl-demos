#ifndef POINTSPRITESCENE_H
#define POINTSPRITESCENE_H

#include "abstractscene.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QMutex>

class Camera;
class Cube;
class Sphere;

#if !defined(Q_OS_MAC)
class QOpenGLFunctions_3_3_Compatibility;
#else
class QOpenGLFunctions_2_1;
class QOpenGLExtension_ARB_instanced_arrays;
class QOpenGLExtension_ARB_draw_instanced;
#endif

class InstancedHistogramScene : public AbstractScene
{
    Q_OBJECT

    Q_PROPERTY( bool updatesEnabled READ updatesEnabled WRITE setUpdatesEnabled NOTIFY updatesEnabledChanged )
    Q_PROPERTY( float frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged )
    Q_PROPERTY( float spatialFrequency READ spatialFrequency WRITE setSpatialFrequency NOTIFY spatialFrequencyChanged )
    Q_PROPERTY( float scale READ scale WRITE setScale NOTIFY scaleChanged )
    Q_PROPERTY( float timeScale READ timeScale WRITE setTimeScale NOTIFY timeScaleChanged )
    Q_PROPERTY( float redBias READ redBias WRITE setRedBias NOTIFY redBiasChanged )
    Q_PROPERTY( float greenBias READ greenBias WRITE setGreenBias NOTIFY greenBiasChanged )
    Q_PROPERTY( float blueBias READ blueBias WRITE setBlueBias NOTIFY blueBiasChanged )

public:
    InstancedHistogramScene( QObject* parent = 0 );

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

    // Properties
    void setUpdatesEnabled( bool enabled );
    bool updatesEnabled() const;

    void setFrequency( float frequency );
    float frequency() const;

    void setSpatialFrequency( float spatialFrequency );
    float spatialFrequency() const;

    void setScale( float scale );
    float scale() const;

    void setTimeScale( float timeScale );
    float timeScale() const;

    void setRedBias( float redBias );
    float redBias() const;

    void setGreenBias( float greenBias );
    float greenBias() const;

    void setBlueBias( float blueBias );
    float blueBias() const;

public slots:
    void toggleUpdates()
    {
        setUpdatesEnabled( !m_updatesEnabled );
    }

signals:
    void updatesEnabledChanged();
    void frequencyChanged();
    void spatialFrequencyChanged();
    void scaleChanged();
    void timeScaleChanged();
    void redBiasChanged();
    void greenBiasChanged();
    void blueBiasChanged();

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();
    void prepareTexture();

#if !defined(Q_OS_MAC)
    QOpenGLFunctions_3_3_Compatibility* m_funcs;
#else
    QOpenGLFunctions_2_1* m_funcs;
    QOpenGLExtension_ARB_instanced_arrays* m_instanceFuncs;
    QOpenGLExtension_ARB_draw_instanced* m_drawInstanced;
#endif

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    Cube* m_cube;

    float m_theta;
    QMatrix4x4 m_modelMatrix;

    // The data arrays and corresponding buffers
    QVector<float> m_data;
    QOpenGLBuffer m_dataBuffer;

    // Sphere for the backgroun
    Sphere* m_sphere;
    QMatrix4x4 m_sphereModelMatrix;
    float m_time;

    bool m_updatesEnabled;

    // Properties
    // Function
    float m_frequency;
    float m_spatialFrequency;

    // Background
    float m_scale;
    float m_timeScale;
    float m_redBias;
    float m_greenBias;
    float m_blueBias;

    QMutex m_mutex;
};

#endif // POINTSPRITESCENE_H
