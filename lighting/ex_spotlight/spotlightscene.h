#ifndef SPOTLIGHTSCENE_H
#define SPOTLIGHTSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;
class Plane;

class SpotLightScene : public AbstractScene
{
public:
    SpotLightScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    void setDeltaCutoffAngle( float delta )
    {
        m_deltaCutoff = delta;
    }

    void setDeltaExponent( float delta )
    {
        m_deltaExponent = delta;
    }

private:
    QString m_fileName;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;
    Plane* m_plane;

    QVector4D m_worldLightPos;
    float m_thetaLight;
    float m_thetaModel;

    float m_cutoffAngle;
    float m_deltaCutoff;
    float m_exponent;
    float m_deltaExponent;
};

#endif // SPOTLIGHTSCENE_H
