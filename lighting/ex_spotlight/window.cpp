#include "window.h"

#include "spotlightscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    SpotLightScene* scene = static_cast<SpotLightScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
            scene->setDeltaCutoffAngle( 0.2f );
            break;

        case Qt::Key_Left:
            scene->setDeltaCutoffAngle( -0.2f );
            break;

        case Qt::Key_Up:
            scene->setDeltaExponent( 0.4f );
            break;

        case Qt::Key_Down:
            scene->setDeltaExponent( -0.4f );
            break;

        default:
            QWindow::keyReleaseEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    SpotLightScene* scene = static_cast<SpotLightScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
        case Qt::Key_Left:
            scene->setDeltaCutoffAngle( 0.0f );
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            scene->setDeltaExponent( 0.0f );
            break;

        default:
            QWindow::keyReleaseEvent( e );
    }
}
