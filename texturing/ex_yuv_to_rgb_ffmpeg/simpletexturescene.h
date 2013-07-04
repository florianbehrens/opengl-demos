#ifndef SIMPLETEXTURESCENE_H
#define SIMPLETEXTURESCENE_H

#include "abstractscene.h"
#include <QObject>

#include "qopenglvertexarrayobject.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include <QSize>
#include <QByteArray>
#include <QProcess>

class QOpenGLFunctions;

class SimpleTextureScene : public AbstractScene
{
    Q_OBJECT

public:
    SimpleTextureScene(const QString &movieFile, QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    void toggleAnimation();

private:
    void prepareShaderProgram( const QString& vertexShader,
                               const QString& fragmentShader );
    void prepareVertexBuffers();
    void prepareVertexArrayObject();
    void prepareTexture();
    int frameByteSize() const;

    QOpenGLShaderProgram m_shader;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_textureCoordinateBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLBuffer m_yTextureBuffer;
    QOpenGLBuffer m_uvTextureBuffer;

    GLuint m_yTextureId;
    GLuint m_uvTextureId;

//    QMatrix4x4 m_modelViewMatrix;

    QString m_movieFile;
    QSize m_frameSize;
    QByteArray m_NV12Buffer;
    QProcess m_videoDecoder;
    QOpenGLFunctions* m_funcs;
};

#endif // SIMPLETEXTURESCENE_H
