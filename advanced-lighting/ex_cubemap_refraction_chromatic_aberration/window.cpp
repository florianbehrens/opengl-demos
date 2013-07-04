#include "window.h"

#include "cubemaprefractionscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    CubeMapRefractionScene* scene = static_cast<CubeMapRefractionScene*>( this->scene() );
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
    CubeMapRefractionScene* scene = static_cast<CubeMapRefractionScene*>( this->scene() );
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
