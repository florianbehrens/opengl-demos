#ifndef AMBIENTOCCLUSIONSCENE_H
#define AMBIENTOCCLUSIONSCENE_H

#include "camerascene.h"
#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

class AmbientOcclusionScene : public CameraScene
{
    Q_OBJECT

public:
    AmbientOcclusionScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

private:
    MaterialPtr createAmbientOcclusionMaterial( const QString& diffuse,
                                                const QString& aoFactors ) const;
    void prepareVertexBuffers();
    void prepareVertexArrayObject();

    QMatrix4x4 m_modelMatrix;


    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // AMBIENTOCCLUSIONSCENE_H
