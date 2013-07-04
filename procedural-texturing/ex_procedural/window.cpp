#include "window.h"

#include "proceduralscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    ProceduralScene* scene = static_cast<ProceduralScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleRotation();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}
