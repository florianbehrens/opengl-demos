#include "window.h"

#include "terraintessellationscene.h"

#include <QCoreApplication>
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
    const float speed = 0.03f;
    TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( this->scene() );
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_Right:
            scene->setSideSpeed( speed );
            break;

        case Qt::Key_Left:
            scene->setSideSpeed( -speed );
            break;

        case Qt::Key_Up:
            scene->setForwardSpeed( speed );
            break;

        case Qt::Key_Down:
            scene->setForwardSpeed( -speed );
            break;

        case Qt::Key_PageUp:
            scene->setVerticalSpeed( speed );
            break;

        case Qt::Key_PageDown:
            scene->setVerticalSpeed( -speed );
            break;

        case Qt::Key_Shift:
            scene->setViewCenterFixed( true );
            break;

        case Qt::Key_Plus:
            scene->setTerrainHorizontalScale( scene->terrainHorizontalScale() + 0.1 );
            break;

        case Qt::Key_Minus:
            scene->setTerrainHorizontalScale( scene->terrainHorizontalScale() - 0.1 );
            break;

        case Qt::Key_Home:
            scene->setTerrainVerticalScale( scene->terrainVerticalScale() + 0.01 );
            break;

        case Qt::Key_End:
            scene->setTerrainVerticalScale( scene->terrainVerticalScale() - 0.01 );
            break;

        default:
            OpenGLWindow::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( this->scene() );
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
            break;

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
        float dx = 1.0f * ( m_pos.x() - m_prevPos.x() );
        float dy = -1.0f * ( m_pos.y() - m_prevPos.y() );
        m_prevPos = m_pos;

        TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( this->scene() );
        scene->pan( dx );
        scene->tilt( dy );
    }

    OpenGLWindow::mouseMoveEvent( e );

}
