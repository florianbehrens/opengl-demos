#include "window.h"

#include "instancedhistogramscene.h"

#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QTimer>

Window::Window( QWindow* parent )
    : QQuickView( parent ),
      m_scene( new InstancedHistogramScene )
{
    QSurfaceFormat format;
#if !defined(Q_OS_MAC)
    format.setMajorVersion( 3 );
    format.setMinorVersion( 3 );
    format.setProfile( QSurfaceFormat::CompatibilityProfile );
#else
    format.setMajorVersion( 2 );
    format.setMinorVersion( 1 );
#endif
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    format.setSamples( 8 );
    setFormat( format );

    setClearBeforeRendering( false );

    QObject::connect( this, &Window::sceneGraphInitialized, this, &Window::initialiseOpenGLScene, Qt::DirectConnection);
    QObject::connect( this, &Window::beforeRendering, this, &Window::renderOpenGLScene, Qt::DirectConnection );
    QObject::connect( this, &Window::sceneGraphInvalidated, this, &Window::cleanupOpenGLScene, Qt::DirectConnection);

    QTimer* timer = new QTimer( this );
    connect( timer, &QTimer::timeout, this, &Window::update );
    timer->start( 16 );

    m_time.start();
}

void Window::initialiseOpenGLScene()
{
    QOpenGLContext* context = openglContext();
    QSurfaceFormat format = context->format();
    qDebug() << "OpenGL version: major =" << format.majorVersion()
             << "minor =" << format.minorVersion();

    m_scene->setContext( context );
    m_scene->initialise();
    m_scene->resize( width(), height() );
}

void Window::renderOpenGLScene()
{
    m_scene->render();
}

void Window::cleanupOpenGLScene()
{
    m_scene->cleanup();
}

void Window::update()
{
    // Update the scene
    float time = m_time.elapsed() / 1000.0f;
    m_scene->update( time );

    // Ask QQuickView to render the scene
    QQuickView::update();
}

void Window::resizeEvent( QResizeEvent* e )
{
    m_scene->resize( width(), height() );
    QQuickView::resizeEvent( e );
}

void Window::keyPressEvent( QKeyEvent* e )
{
    InstancedHistogramScene* scene = static_cast<InstancedHistogramScene*>( m_scene );
    const float speed = 5.0f;
    switch ( e->key() )
    {
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

        default:
            QQuickView::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    InstancedHistogramScene* scene = static_cast<InstancedHistogramScene*>( m_scene );
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
            QQuickView::keyReleaseEvent( e );
    }
}
