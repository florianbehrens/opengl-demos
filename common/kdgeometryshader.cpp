#include "kdgeometryshader.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QScopedArrayPointer>
#include <QDebug>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#ifdef QT_OPENGL_ES_2
#error "There are no geometry shaders in OpenGL ES 2"
#endif

//#define KDGEOMETRYSHADER_DEBUG

#ifdef KDGEOMETRYSHADER_DEBUG
#define qGeometryShaderDebug qDebug
#else
#define qGeometryShaderDebug QT_NO_QDEBUG_MACRO
#endif

KDGeometryShader::KDGeometryShader()
    : m_shaderId( 0 )
{
}

KDGeometryShader::~KDGeometryShader()
{
    QOpenGLFunctions * funcs = QOpenGLContext::currentContext()->functions();
    funcs->glDeleteShader( m_shaderId );
}

void KDGeometryShader::create( ShaderStage stage )
{
    QOpenGLFunctions * funcs = QOpenGLContext::currentContext()->functions();
    m_shaderId = funcs->glCreateShader( stage );
}

bool KDGeometryShader::isCreated() const
{
    return ( m_shaderId != 0 );
}


bool KDGeometryShader::loadShaderSource( const QByteArray &source )
{
    QOpenGLFunctions * funcs = QOpenGLContext::currentContext()->functions();
    const char *codeStrings[ 1 ] = { source.constData() };
    const GLint codeLengths[ 1 ] = { source.length() };
    funcs->glShaderSource( m_shaderId, 1, codeStrings, codeLengths );
    return true;
}

bool KDGeometryShader::compileShader()
{
    qGeometryShaderDebug() << Q_FUNC_INFO;
    QOpenGLFunctions * funcs = QOpenGLContext::currentContext()->functions();
    funcs->glCompileShader( m_shaderId );
    GLint result = 0;
    funcs->glGetShaderiv( m_shaderId, GL_COMPILE_STATUS, &result );
#ifndef _DEBUG
    if ( !result )
    {
#endif
        GLint logLength = 0;
        funcs->glGetShaderiv( m_shaderId, GL_INFO_LOG_LENGTH, &logLength );
        if ( logLength )
        {
            QScopedArrayPointer< char > logString( new char[ logLength ] );
            GLint written;
            funcs->glGetShaderInfoLog( m_shaderId, logLength, &written, logString.data() );
            qGeometryShaderDebug() << logString.data();
        }
#ifndef _DEBUG
        Q_ASSERT( false );
    }
#endif
    return result;
}

void KDGeometryShader::attachToProgram( const QOpenGLShaderProgramPtr& program )
{
    QOpenGLFunctions * funcs = QOpenGLContext::currentContext()->functions();
    Q_ASSERT( program );
    funcs->glAttachShader( program->programId(), m_shaderId );
}
