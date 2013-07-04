#include "window.h"

#include "uvmapscene.h"

#include <QKeyEvent>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* ev )
{
    UVmapScene* scene = static_cast<UVmapScene*>( this->scene() );
    switch ( ev->key() )
    {
        case Qt::Key_Space:
            scene->toggleAnimation();
            break;

        default:
            OpenGLWindow::keyPressEvent( ev );
    }
}
