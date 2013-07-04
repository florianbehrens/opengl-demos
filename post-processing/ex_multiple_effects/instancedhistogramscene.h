#ifndef INSTANCEDHISTOGRAMSCENE_H
#define INSTANCEDHISTOGRAMSCENE_H

#include "abstractscene.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Camera;
class Cube;
class FullScreenQuad;
class Sphere;

#if !defined(Q_OS_MAC)
class QOpenGLFunctions_3_3_Core;
#else
class QOpenGLFunctions_3_2_Core;
class QOpenGLExtension_ARB_instanced_arrays;
#endif

class InstancedHistogramScene : public AbstractScene
{
    Q_OBJECT

public:
    InstancedHistogramScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    void toggleUpdates()
    {
        m_updatesEnabled = !m_updatesEnabled;
    }

    // Camera motion control
    void setSideSpeed( float vx ) { m_vx = vx; }
    void setVerticalSpeed( float vy ) { m_vy = vy; }
    void setForwardSpeed( float vz ) { m_vz = vz; }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }

    // Camera orientation control
    void pan( float angle ) { m_panAngle = angle; }
    void tilt( float angle ) { m_tiltAngle = angle; }

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    void switchToFBO( GLuint fbo );
    void switchToDefaultFramebuffer();

    void renderScene();
    void renderBlurEffect( int pass );
    void renderTelevisionEffect();

    void prepareFBO1( GLuint textureId );
    void prepareFBO2( GLuint textureId );

    MaterialPtr createBlurMaterial1( const QString& vertexShader,
                                     const QString& fragmentShader);
    MaterialPtr createBlurMaterial2( const MaterialPtr& material1 );
    MaterialPtr createTelevisionMaterial( const QString& vertexShader,
                                          const QString& fragmentShader,
                                          const MaterialPtr &material );

#if !defined(Q_OS_MAC)
    QOpenGLFunctions_3_3_Core* m_funcs;
#else
    QOpenGLFunctions_3_2_Core* m_funcs;
    QOpenGLExtension_ARB_instanced_arrays* m_instanceFuncs;
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

    // Sphere for the background
    Sphere* m_sphere;
    QMatrix4x4 m_sphereModelMatrix;
    float m_time;

    bool m_updatesEnabled;

    // The FBO handles and depth buffer handle
    GLuint m_fbo1;
    GLuint m_fbo2;
    GLuint m_depthBuffer;

    // The generated texture handle
    GLuint m_renderTexture;

    // Fullscreen quad related members
    FullScreenQuad* m_fsQuad;
    MaterialPtr m_postProcBlurMaterial1;
    MaterialPtr m_postProcBlurMaterial2;
    MaterialPtr m_postProcTelevisionMaterial;
    int m_blurPass;

    // Window and texture sizes
    QSize m_windowSize;
    QSize m_textureSize;
};

#endif // INSTANCEDHISTOGRAMSCENE_H
