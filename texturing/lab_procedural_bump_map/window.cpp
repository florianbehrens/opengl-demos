#include "window.h"

#include "bumpscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window(const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    BumpScene* scene = static_cast<BumpScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleRotation();
            break;

        case Qt::Key_T:
            scene->toggleNormals();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}


