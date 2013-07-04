#include "window.h"

#include "normalmapscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    NormalMapScene* scene = static_cast<NormalMapScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleRotation();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}
