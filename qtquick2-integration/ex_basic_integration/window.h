#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QQuickView>

#include "qopenglvertexarrayobject.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QTime>

class AbstractScene;

class QOpenGLShaderProgram;

class Window : public QQuickView
{
    Q_OBJECT

public:
    explicit Window( QWindow* parent = 0 );

public slots:
    void initialiseOpenGLScene();
    void renderOpenGLScene();
    void cleanupOpenGLScene();

private:
    AbstractScene* m_scene;
    QTime m_time;
};

#endif // MYWINDOW_H
