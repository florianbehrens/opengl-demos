#include "window.h"

#include "transformscene.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

const double dt = 1.0 / 30.0;

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
    m_zVelocity = 0.0;
    m_yawRate = 0.0;
    
    m_moveTimer.setInterval(dt * 1000);
    connect(&m_moveTimer, SIGNAL(timeout()), this, SLOT(onMoveTimeout()));
    m_moveTimer.start();
}

void Window::onMoveTimeout()
{
    TransformScene* scene = static_cast<TransformScene*>( this->scene() );
        
    
    
    QMatrix4x4 movement;
    movement.setToIdentity();
    
    movement.rotate(m_yawRate * dt, QVector3D(0, 1, 0));
    movement.translate(0, 0, m_zVelocity * dt);
    
    QMatrix4x4 m = scene->viewMatrix();
    m = movement * m;
    scene->setViewMat(m);
}

void Window::keyPressEvent( QKeyEvent* e )
{
    
    switch ( e->key() )
    {
        case Qt::Key_W:
            m_zVelocity = 15.0;
            break;

        case Qt::Key_A:
            m_yawRate = -60;
            break;

        case Qt::Key_S:
            m_zVelocity = -5.0;
            break;

        case Qt::Key_D:
            m_yawRate = 60;
            break;

        default:
            QWindow::keyPressEvent( e );
            return;
    }
    
    
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_W:
        case Qt::Key_S:
            m_zVelocity = 0.0;
            break;

        case Qt::Key_A:
        case Qt::Key_D:
            m_yawRate = 0.0;
            break;

        default:
            QWindow::keyReleaseEvent( e );
    }
}
