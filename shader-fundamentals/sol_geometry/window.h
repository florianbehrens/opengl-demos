#ifndef WINDOW_H
#define WINDOW_H

#include "openglwindow.h"

class Window : public OpenGLWindow
{
    Q_OBJECT

public:
    Window( const QSurfaceFormat& format, QScreen* screen = 0 );

protected:
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );

private:
    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
};

#endif // WINDOW_H
