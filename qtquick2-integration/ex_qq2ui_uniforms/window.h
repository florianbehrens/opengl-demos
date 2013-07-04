#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QQuickView>

#include "qopenglvertexarrayobject.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QTime>

class InstancedHistogramScene;

class QOpenGLShaderProgram;

class Window : public QQuickView
{
    Q_OBJECT

public:
    explicit Window( QWindow* parent = 0 );

    InstancedHistogramScene* scene() const { return m_scene; }

public slots:
    void initialiseOpenGLScene();
    void renderOpenGLScene();
    void cleanupOpenGLScene();
    void update();

protected:
    void resizeEvent( QResizeEvent* e );
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );

private:
    InstancedHistogramScene* m_scene;
    QTime m_time;
};

#endif // MYWINDOW_H
