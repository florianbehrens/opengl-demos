#include "window.h"

#include "directionalscene.h"

#include <QDir>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* ev )
{
    switch ( ev->key() )
    {
        case Qt::Key_Space:
            qobject_cast<DirectionalScene*>( scene() )->toggleLightType();
            break;

        default:
            OpenGLWindow::keyPressEvent( ev );
    }
}
