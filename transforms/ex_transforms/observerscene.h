#ifndef OBSERVERSCENE_H
#define OBSERVERSCENE_H

#include "transformscene.h"

class Grid;

class ObserverScene : public TransformScene
{
public:
    ObserverScene(TransformScene* scn);
    virtual ~ObserverScene();

    virtual void initialise();
    virtual void render();

private:
    TransformScene* m_peer;

    QOpenGLBuffer m_cameraPositionBuffer;
    QVector<float> m_cameraVerts;
};

#endif // OBSERVERSCENE_H
