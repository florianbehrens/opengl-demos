#include "window.h"

#include "cubemapreflectionscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window(const QSurfaceFormat &format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    CubeMapReflectionScene* scene = static_cast<CubeMapReflectionScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
            scene->setYawSpeed( 0.3f );
            break;

        case Qt::Key_Left:
            scene->setYawSpeed( -0.3f );
            break;

        case Qt::Key_Up:
            scene->setPitchSpeed( 0.3f );
            break;

        case Qt::Key_Down:
            scene->setPitchSpeed( -0.3f );
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    CubeMapReflectionScene* scene = static_cast<CubeMapReflectionScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
            scene->setYawSpeed( 0.0f );
            break;

        case Qt::Key_Left:
            scene->setYawSpeed( 0.0f );
            break;

        case Qt::Key_Up:
            scene->setPitchSpeed( 0.0f );
            break;

        case Qt::Key_Down:
            scene->setPitchSpeed( 0.0f );
            break;

        default:
            OpenGLWindow::keyReleaseEvent( e );
    }
}
