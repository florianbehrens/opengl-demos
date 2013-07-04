#ifndef DIFFUSESCENE_H
#define DIFFUSESCENE_H

#include "abstractscene.h"
#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Torus;

class DiffuseScene : public AbstractScene
{
public:
    DiffuseScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

private:
    void prepareShaderProgram( const QString& vertexShader, const QString& fragmentShader );

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    QOpenGLShaderProgramPtr m_program;
    Torus* m_torus;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // DIFFUSESCENE_H
