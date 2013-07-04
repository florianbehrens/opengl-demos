#ifndef TERRAINTESSELLATIONSCENE_H
#define TERRAINTESSELLATIONSCENE_H

#include <QObject>
#include "abstractscene.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "qopenglvertexarrayobject.h"
#include <QMatrix4x4>

class QOpenGLFunctions_4_0_Core;

class Camera;

class TerrainTessellationScene : public AbstractScene
{
    Q_OBJECT

public:
    TerrainTessellationScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    // Camera motion control
    void setSideSpeed( float vx ) { m_vx = vx; }
    void setVerticalSpeed( float vy ) { m_vy = vy; }
    void setForwardSpeed( float vz ) { m_vz = vz; }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }

    // Camera orientation control
    void pan( float angle ) { m_panAngle = angle; }
    void tilt( float angle ) { m_tiltAngle = angle; }

    // Terrain scales
    void setTerrainHorizontalScale( float horizontalScale ) { m_horizontalScale = horizontalScale; }
    float terrainHorizontalScale() const { return m_horizontalScale; }
    void setTerrainVerticalScale( float verticalScale ) { m_verticalScale = verticalScale; }
    float terrainVerticalScale() const { return m_verticalScale; }

private:
    void prepareVertexBuffers( QSize heightMapSize );
    void prepareVertexArrayObject();

    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    QMatrix4x4 m_viewportMatrix;

    // The terrain "object"
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_patchBuffer;
    int m_patchCount;
    MaterialPtr m_material;

    // Terrain rendering controls
    QMatrix4x4 m_modelMatrix;
    float m_horizontalScale;
    float m_verticalScale;

    // Angle of camera about vertical axis
    float m_theta;

    QOpenGLFunctions_4_0_Core* m_funcs;
};

#endif // TERRAINTESSELLATIONSCENE_H
