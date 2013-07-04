#ifndef SKYBOX_H
#define SKYBOX_H

#include <QObject>

#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglvertexarrayobject.h"

class SkyBox : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString cubeMapBaseName READ cubeMapBaseName WRITE setCubeMapBaseName )
    Q_PROPERTY( bool seamlessCubeMap READ isSeamlessCubeMapEnabled WRITE setSeamlessCubeMapEnabled )

public:
    explicit SkyBox( QObject* parent = 0 );

    QString cubeMapBaseName() const
    {
        return m_cubeMapBaseName;
    }

    bool isSeamlessCubeMapEnabled() const
    {
        return m_seamlessCubeMap;
    }

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const { return m_material; }

    void create();
    void render( const QMatrix4x4& mvp );

    bool isCreated() const { return m_vao.objectId() != 0; }

public slots:
    void setCubeMapBaseName( const QString& cubeMapBaseName );
    void setSeamlessCubeMapEnabled( bool seamlessEnabled );

private:
    void updateMaterialCubeMap();

    QString m_cubeMapBaseName;
    bool m_seamlessCubeMap;

    // Buffers for vertex and index data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_indexBuffer;

    // Shader program
    //QOpenGLShaderProgram m_shader;
    MaterialPtr m_material;

    // Vertex array object to quickly set render state
    QOpenGLVertexArrayObject m_vao;
    int m_indexCount;

    // Texture Id
    //GLuint m_texID;
};

#endif // SKYBOX_H
