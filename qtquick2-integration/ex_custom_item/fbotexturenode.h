#ifndef FBOTEXTURENODE_H
#define FBOTEXTURENODE_H

#include <QObject>
#include <qsgsimpletexturenode.h>

#include <QQuickWindow>
#include <QTime>

class AbstractScene;
class LogoRenderer;

class FboTextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    explicit FboTextureNode( QQuickWindow* parent = 0 );
    ~FboTextureNode();

    void setScene( AbstractScene* scene ) { m_scene = scene; }
    AbstractScene* scene() const { return m_scene; }

private slots:
    void renderToTexture();

private:
    QQuickWindow* m_window;
    QOpenGLFramebufferObject* m_fbo;
    QSGTexture* m_texture;

    AbstractScene* m_scene;
    QTime m_time;
};

#endif // FBOTEXTURENODE_H
