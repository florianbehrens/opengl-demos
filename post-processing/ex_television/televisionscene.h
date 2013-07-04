#ifndef TELEVISIONSCENE_H
#define TELEVISIONSCENE_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class FullScreenQuad;
class Mesh;

class QOpenGLFunctions_3_2_Core;

class TelevisionScene : public AbstractScene
{
    Q_OBJECT

public:
    TelevisionScene( const QString& fileName, QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void switchToFBO();
    void switchToDefaultFramebuffer();

    void renderScene();
    void postProcessScene();

    void prepareFBO( GLuint textureId );
    MaterialPtr createPostProcessMaterial(const QString& vertexShader,
                                          const QString& fragmentShader);

    // OpenGL functions
    QOpenGLFunctions_3_2_Core* m_funcs;

    // Window and texture sizes
    QSize m_windowSize;
    QSize m_textureSize;

    // The FBO handle and depth buffer handle
    GLuint m_fbo;
    GLuint m_depthBuffer;

    // The generated texture handle
    GLuint m_renderTexture;

    // The model we will render to a texture
    QString m_fileName;
    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    Mesh* m_mesh;
    float m_theta;

    // Fullscreen quad related members
    FullScreenQuad* m_fsQuad;
    float m_edgeThreshold;

    // Time in seconds
    float m_time;
};

#endif // TELEVISIONSCENE_H
