#ifndef DIRECTIONALSCENE_H
#define DIRECTIONALSCENE_H

#include "abstractscene.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Mesh;

class DirectionalScene : public AbstractScene
{
    Q_OBJECT

public:
    DirectionalScene( const QString& fileName );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    void toggleLightType()
    {
        if ( m_lightType == Directional )
        {
            m_lightType = Point;
            qDebug() << "Point light";
        }
        else
        {
            m_lightType = Directional;
            qDebug() << "Directional light";
        }
    }

private:
    QString m_fileName;

    enum LightType
    {
        Point,
        Directional
    };
    LightType m_lightType;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Mesh* m_mesh;

    // Angle of camera about vertical axis
    float m_theta;
};

#endif // DIRECTIONALSCENE_H
