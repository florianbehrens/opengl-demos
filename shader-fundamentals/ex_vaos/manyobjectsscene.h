#ifndef MANYOBJECTSSSCENE_H
#define MANYOBJECTSSSCENE_H

#include "camerascene.h"
#include "qopenglvertexarrayobject.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Cube;
class Sphere;
class Torus;

class ManyObjectsScene : public CameraScene
{
public:
    ManyObjectsScene( );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

private:
	void setupMatrices(QOpenGLShaderProgramPtr shader, const QMatrix4x4& modelMatrix);
	
    QMatrix4x4 m_modelMatrix;

    Cube* m_cube;
    Sphere* m_sphere;
    Torus* m_torus;
	
    // Angle of camera about vertical axis
    float m_theta;
};

#endif // MANYOBJECTSSSCENE_H
