#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include "camerascene.h"
#include "material.h"
#include "qopenglvertexarrayobject.h"
#include "grid.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Mesh;

class TransformScene : public CameraScene
{
public:
    TransformScene();

    void setModelMat(int index, const QMatrix4x4& model);

    QMatrix4x4 modelMatrix(int instanceID) const
        { return m_modelMats[instanceID]; }

    int numObjects() const
        { return m_modelMats.count(); }

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    QSize size() const { return m_size; }
protected:
    void drawMesh(Mesh* mesh, const QMatrix4x4& pos);

    void setupRenderState();
    void prepareShaderProgram( QOpenGLShaderProgramPtr program,
                               const QString& vertexShader,
                               const QString& fragmentShader );

    QOpenGLShaderProgramPtr m_program;

    Mesh* m_mesh;
    Mesh* m_originMarker;
    QVector<QMatrix4x4> m_modelMats;
    QSize m_size;

    Grid* m_grid;
    QOpenGLShaderProgramPtr m_gridProgram;
};

#endif // TRANSFORMSCENE_H
