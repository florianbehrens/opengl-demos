#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include <QWindow>
#include <QTime>

class AbstractScene;

class BasicOpenGLWindow : public QWindow
{
    Q_OBJECT

public:
    explicit BasicOpenGLWindow( const QSurfaceFormat& format,
                                QScreen* parent = 0 );

    QOpenGLContext* context() const { return m_context; }

    void setScene( AbstractScene* scene );
    AbstractScene* scene() const { return m_scene; }
    
protected:
    virtual void initializeGL();
    virtual void resizeGL();
    virtual void paintGL();

protected slots:
    virtual void updateScene();

    void resizeEvent( QResizeEvent* e );
    void keyPressEvent( QKeyEvent* e );

private:
    QOpenGLContext* m_context;
    AbstractScene* m_scene;
    
    QTime m_time;
};

#endif // BASICWINDOW_H
