#ifndef WINDOW_H
#define WINDOW_H

#include "openglwindow.h"
#include <QTimer>

class Window : public OpenGLWindow
{
    Q_OBJECT

public:
    Window( const QSurfaceFormat& format, QScreen* screen = 0 );

protected:
    void keyPressEvent( QKeyEvent* ev );
    void keyReleaseEvent( QKeyEvent* ev );
    
    double m_zVelocity;
    double m_yawRate;
    
     QTimer m_moveTimer;
protected slots:
    void onMoveTimeout();
   
};

#endif // WINDOW_H
