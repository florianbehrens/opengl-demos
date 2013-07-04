#ifndef MANYLIGHTSSCENE_H
#define MANYLIGHTSSCENE_H

#include "camerascene.h"
#include "qopenglvertexarrayobject.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;
class Cube;
class Sphere;
class Plane;

class Light
{
public:
    void bindUniforms(QOpenGLShaderProgramPtr shader, QString prefix);
    
    QVector4D positionOrDirection;
    QVector3D intensity;
    double cutoffDistance;  
};

class SpotLight
{
public:
    void bindUniforms(QOpenGLShaderProgramPtr shader, QString prefix,
      const QMatrix4x4& modelView);
    
    QVector4D position;
    QVector4D direction;
    QVector3D intensity;
    double cutoffAngle;
    double cutoffExponent;
};

class FogInfo
{
public:
    void bindUniforms(QOpenGLShaderProgramPtr shader);
    
    QVector3D color; 
    double minDistance;
    double maxDistance;
};

class ManyLightsScene : public CameraScene
{
public:
    ManyLightsScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

private:
    void prepareLights(int numLights, int numSpots);
    void bindSpotLights(QOpenGLShaderProgramPtr shader, const QMatrix4x4& modelViewMatrix);
    
    QString m_fileName;

    QMatrix4x4 m_modelMatrix;

    Mesh* m_mesh;
    Cube* m_cube;
    Sphere* m_sphere;
    Plane* m_ground;
    
    void renderSphereGrid();
    
    FogInfo m_fog;
    
    QVector<Light> m_lights;
    QVector<SpotLight> m_spotLights;
    
    // Angle of camera about vertical axis
    float m_theta;
};

#endif // MANYLIGHTSSCENE_H
