#include "window.h"

#include "alphamapscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window(const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    AlphaMapScene* scene = static_cast<AlphaMapScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleCubeRotation();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}
