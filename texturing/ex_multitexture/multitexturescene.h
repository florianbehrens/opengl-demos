#ifndef MULTITEXTURESCENE_H
#define MULTITEXTURESCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>

class Cube;

class MultiTextureScene : public CameraScene
{
    Q_OBJECT

public:
    MultiTextureScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();

private:
    MaterialPtr createMultiTextureMaterial( const QString& texture0,
                                            const QString& texture1 );

    QMatrix4x4 m_modelMatrix;


    Cube* m_cube;
    float m_theta;
    float m_factor;
};

#endif // MULTITEXTURESCENE_H
