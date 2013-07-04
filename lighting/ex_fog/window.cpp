#include "window.h"

#include "fogscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen ),
      m_leftButtonPressed( false )
{
}

void Window::keyPressEvent( QKeyEvent* e )
{
    FogScene* scene = static_cast<FogScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
            scene->setSideSpeed( 0.3f );
            break;

        case Qt::Key_Left:
            scene->setSideSpeed( -0.3f );
            break;

        case Qt::Key_Up:
            scene->setForwardSpeed( 0.3f );
            break;

        case Qt::Key_Down:
            scene->setForwardSpeed( -0.3f );
            break;

        case Qt::Key_PageUp:
            scene->setVerticalSpeed( 0.3f );
            break;

        case Qt::Key_PageDown:
            scene->setVerticalSpeed( -0.3f );
            break;

        case Qt::Key_Shift:
            scene->setViewCenterFixed( true );

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    FogScene* scene = static_cast<FogScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Right:
        case Qt::Key_Left:
            scene->setSideSpeed( 0.0f );
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            scene->setForwardSpeed( 0.0f );
            break;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            scene->setVerticalSpeed( 0.0f );
            break;

        case Qt::Key_Shift:
            scene->setViewCenterFixed( false );

        default:
            OpenGLWindow::keyReleaseEvent( e );
    }
}

void Window::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = true;
        m_pos = m_prevPos = e->pos();
    }
    OpenGLWindow::mousePressEvent( e );
}

void Window::mouseReleaseEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
        m_leftButtonPressed = false;
    OpenGLWindow::mouseReleaseEvent( e );
}

void Window::mouseMoveEvent( QMouseEvent* e )
{
    if ( m_leftButtonPressed )
    {
        m_pos = e->pos();
        float dx = -1.0f * ( m_pos.x() - m_prevPos.x() );
        float dy = -1.0f * ( m_pos.y() - m_prevPos.y() );
        m_prevPos = m_pos;

        FogScene* scene = static_cast<FogScene*>( this->scene() );
        scene->pan( dx );
        scene->tilt( dy );
    }
    OpenGLWindow::mouseMoveEvent( e );
}
