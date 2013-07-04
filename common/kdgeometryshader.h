#ifndef KDGEOMETRYSHADER_H
#define KDGEOMETRYSHADER_H

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QSharedPointer>

typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class KDGeometryShader : public QObject
{
public:
    KDGeometryShader();
    ~KDGeometryShader();

    enum ShaderStage
    {
        GeometryShader = GL_GEOMETRY_SHADER
#if !defined(Q_OS_MAC)
      , TessellationControlShader = GL_TESS_CONTROL_SHADER
      , TessellationEvaluationShader = GL_TESS_EVALUATION_SHADER
#endif
    };

    void create( ShaderStage stage );
    bool isCreated() const;
    bool loadShaderSource( const QByteArray &source );
    bool compileShader();
    void attachToProgram( const QOpenGLShaderProgramPtr& program );

private:
    GLuint m_shaderId;
};

#endif // KDGEOMETRYSHADER_H
