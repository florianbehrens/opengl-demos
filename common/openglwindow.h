#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>
#include <QTime>

class CameraScene;
class AbstractScene;
class CameraController;
class QOpenGLDebugMessage;
class OpenGLDebugMessageWindow;

class OpenGLWindow : public QWindow
{
    Q_OBJECT

public:
    explicit OpenGLWindow( const QSurfaceFormat& format,
                           QScreen* parent = 0 );

    QOpenGLContext* context() const { return m_context; }

    void setScene( AbstractScene* scene );
    AbstractScene* scene() const { return m_scene; }
    
protected:
    virtual void initializeGL();
    virtual void resizeGL();
    virtual void paintGL();

    virtual void keyReleaseEvent( QKeyEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void mouseReleaseEvent( QMouseEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );

protected slots:
    virtual void updateScene();

    void resizeEvent( QResizeEvent* e );
    void keyPressEvent( QKeyEvent* e );

    void messageLogged(const QOpenGLDebugMessage &message);

private:
    QOpenGLContext* m_context;
    AbstractScene* m_scene;
    CameraController* m_controller;
    OpenGLDebugMessageWindow* m_debugWindow;
    
    QTime m_time;
};

#endif // OPENGLWINDOW_H
