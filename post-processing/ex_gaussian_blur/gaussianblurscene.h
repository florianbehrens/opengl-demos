#ifndef GAUSSIANBLURSCENE_H
#define GAUSSIANBLURSCENE_H

#include <abstractscene.h>
#include <material.h>

#include <QMatrix4x4>

class FullScreenQuad;
class Mesh;

class QOpenGLFunctions_3_2_Core;

class GaussianBlurScene : public AbstractScene
{
    Q_OBJECT

public:
    GaussianBlurScene( const QString& fileName, QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void switchToFBO( GLuint fbo );
    void switchToDefaultFramebuffer();

    void renderScene();
    void postProcessScene( int pass );

    void prepareFBO1( GLuint textureId );
    void prepareFBO2( GLuint textureId );

    MaterialPtr createPostProcessMaterial1( const QString& vertexShader,
                                            const QString& fragmentShader);
    MaterialPtr createPostProcessMaterial2( const MaterialPtr& material1 );

    // OpenGL functions
    QOpenGLFunctions_3_2_Core* m_funcs;

    // Window and texture sizes
    QSize m_windowSize;
    QSize m_textureSize;

    // The FBO handles and depth buffer handle
    GLuint m_fbo1;
    GLuint m_fbo2;
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
    MaterialPtr m_postProcMaterial1;
    MaterialPtr m_postProcMaterial2;
    int m_pass;
    int m_blurPasses;
};

#endif // GAUSSIANBLURSCENE_H
