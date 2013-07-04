#include "window.h"

#include "basicusagescene.h"

#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QTimer>

Window::Window( QWindow* parent )
    : QQuickView( parent ),
      m_scene( new BasicUsageScene )
{
    setClearBeforeRendering( false );

    QObject::connect( this, &Window::sceneGraphInitialized,
                      this, &Window::initialiseOpenGLScene, Qt::DirectConnection);
    QObject::connect( this, &Window::beforeRendering,
                      this, &Window::renderOpenGLScene, Qt::DirectConnection );
    QObject::connect( this, &Window::sceneGraphInvalidated,
                      this, &Window::cleanupOpenGLScene, Qt::DirectConnection);

    QTimer* timer = new QTimer( this );
    connect( timer, &QTimer::timeout, this, &Window::update );
    timer->start( 16 );

    QSurfaceFormat format;
    format.setMajorVersion( 3 );
    format.setMinorVersion( 3 );
    format.setProfile( QSurfaceFormat::CompatibilityProfile );
    format.setStencilBufferSize(8);
    setFormat( format );

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
