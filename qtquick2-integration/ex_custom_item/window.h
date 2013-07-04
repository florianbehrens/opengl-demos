#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QQuickView>

class AbstractScene;

class QOpenGLContext;

class Window : public QQuickView
{
    Q_OBJECT

public:
    Window( QWindow* parent = 0 );
};

#endif // GLWIDGET_H
