#include "window.h"

#include "specularmapscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    SpecularMapScene* scene = static_cast<SpecularMapScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Space:
            scene->toggleCubeRotation();
            break;

        case Qt::Key_M:
            scene->toggleMultiSampling();
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}
