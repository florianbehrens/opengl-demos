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

};

#endif // WINDOW_H
