#include "window.h"

#include "antialiasingscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    AntiAliasingScene* scene = static_cast<AntiAliasingScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleRotation();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}
