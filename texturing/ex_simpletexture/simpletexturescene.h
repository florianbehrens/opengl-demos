#ifndef SIMPLETEXTURESCENE_H
#define SIMPLETEXTURESCENE_H

#include "abstractscene.h"
#include <QObject>

#include "qopenglvertexarrayobject.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class QOpenGLFunctions;
class QPropertyAnimation;

class SimpleTextureScene : public AbstractScene
{
    Q_OBJECT

    Q_PROPERTY( float filterPosition READ filterPosition WRITE setFilterPosition )

public:
    SimpleTextureScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    void setFilterPosition( float x );
    float filterPosition();

    void toggleAnimation();

private:
    void prepareShaderProgram( const QString& vertexShader,
                               const QString& fragmentShader );
    void prepareVertexBuffers();
    void prepareVertexArrayObject();
    void prepareTexture( const QString& fileName );

    QOpenGLShaderProgram m_shader;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_textureBuffer;
    QOpenGLBuffer m_indexBuffer;

    GLuint m_textureId;

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_modelViewMatrix;

    QPropertyAnimation* m_propertyAnimation;
    float m_filterPosition;

    QOpenGLFunctions* m_funcs;
};

#endif // SIMPLETEXTURESCENE_H
