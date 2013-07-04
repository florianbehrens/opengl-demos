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
                      this, &Window::renderOpenGLScene, Qt::DirectConnection);
    QObject::connect( this, &Window::sceneGraphInvalidated,
                      this, &Window::cleanupOpenGLScene, Qt::DirectConnection);
}

void Window::initialiseOpenGLScene()
{
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
