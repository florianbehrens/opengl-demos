#include "openglwindow.h"

#include "camerascene.h"
#include "cameracontroller.h"

#include "opengldebugmessagemodel.h"
#include "opengldebugmessagewindow.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QTimer>

OpenGLWindow::OpenGLWindow( const QSurfaceFormat& format,
                            QScreen* screen )
    : QWindow( screen ),
      m_context( 0 ),
      m_scene( 0 ),
      m_controller( 0 ),
      m_debugWindow( 0 )
{
    m_controller = new CameraController;

    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );

    // Request a full screen button (if available)
#if !defined(Q_OS_WIN)
    setFlags( flags() | Qt::WindowFullscreenButtonHint );
#endif

    // Set an initial window size (cocoa qpa needs this)
    resize( 800, 600 );

    QSurfaceFormat actualFormat = format;

    if (qgetenv("OPENGL_DEBUG").toInt())
        actualFormat.setOption(QSurfaceFormat::DebugContext);

    // Create the native window
    setFormat( actualFormat );
    create();

    // Create an OpenGL context
    m_context = new QOpenGLContext;
    m_context->setFormat( actualFormat );
    m_context->create();
}

void OpenGLWindow::setScene( AbstractScene* scene )
{
    // We take ownership of the scene
    Q_ASSERT( scene );
    m_scene = scene;
    m_scene->setParent( this );

    // Initialise the scene
    m_scene->setContext( m_context );
    initializeGL();

    // Set initial window size
    resize( 800, 600 );

    // This timer drives the scene updates
    QTimer* timer = new QTimer( this );
    connect( timer, &QTimer::timeout, this, &OpenGLWindow::updateScene );
    timer->start( 16 );

    CameraScene* camScene = qobject_cast<CameraScene*>( scene );
    if ( camScene )
    {
        m_controller->setCamera( camScene->camera() );
    }
}

void OpenGLWindow::initializeGL()
{
    m_context->makeCurrent( this );

    if (qgetenv("OPENGL_DEBUG").toInt()) {
        QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);
        if (logger->initialize()) {
            const QList<QOpenGLDebugMessage> startupMessages = logger->loggedMessages();
            if (qApp && qApp->inherits("QApplication")) {
                OpenGLDebugMessageModel *model = new OpenGLDebugMessageModel(this);
                model->setMessages(startupMessages);

                connect(logger, &QOpenGLDebugLogger::messageLogged,
                        model, &OpenGLDebugMessageModel::appendMessage);
                logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);

                m_debugWindow = new OpenGLDebugMessageWindow;
                m_debugWindow->setModel(model);

                qDebug() << "Debug mode activated, press F5 to open the debug log window";
            } else {
                connect(logger, &QOpenGLDebugLogger::messageLogged,
                        this, &OpenGLWindow::messageLogged);
                qDebug() << "Debug log requested, but QApplication not used -- dumping OpenGL messages via qDebug";
                if (!startupMessages.isEmpty())
                    qDebug() << "Debug messages logged on startup:" << startupMessages;
            }
        } else {
            qDebug() << "Debugging requested but logger failed to initialize";
        }
    }

    m_scene->initialise();

    m_time.start();
}

void OpenGLWindow::resizeGL()
{
    m_context->makeCurrent( this );
    m_scene->resize( width(), height() );
}

void OpenGLWindow::paintGL()
{
    if ( !isExposed() )
        return;

    // Make the context current
    m_context->makeCurrent( this );

    if ( m_controller->isMultisampleEnabled() )
        glEnable( GL_MULTISAMPLE );
    else
        glDisable( GL_MULTISAMPLE );

    // Do the rendering (to the back buffer)
    m_scene->render();

    // Swap front/back buffers
    m_context->swapBuffers( this );
}

void OpenGLWindow::updateScene()
{
    float time = m_time.elapsed() / 1000.0f;
    m_controller->update( time );
    m_scene->update( time );
    paintGL();
}

void OpenGLWindow::resizeEvent( QResizeEvent* e )
{
    Q_UNUSED( e );
    if ( m_context )
        resizeGL();
}

void OpenGLWindow::keyPressEvent( QKeyEvent* e )
{
    if (m_controller->keyPressEvent(e))
      return;

    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QGuiApplication::quit();
            break;
        case Qt::Key_F5:
        qDebug() << "SHOWING THE DEBUG WINDOW";
            if (m_debugWindow)
                m_debugWindow->show();
        default:
            QWindow::keyPressEvent( e );
    }
}

void OpenGLWindow::messageLogged(const QOpenGLDebugMessage &message)
{
    qDebug() << message;
}

void OpenGLWindow::keyReleaseEvent( QKeyEvent* e )
{
    if (m_controller->keyReleaseEvent(e))
      return;

    QWindow::keyReleaseEvent(e);
}

void OpenGLWindow::mousePressEvent( QMouseEvent* e )
{
    m_controller->mousePressEvent(e);
}

void OpenGLWindow::mouseReleaseEvent( QMouseEvent* e )
{
    m_controller->mouseReleaseEvent(e);
}

void OpenGLWindow::mouseMoveEvent( QMouseEvent* e )
{
    m_controller->mouseMoveEvent(e);
}
