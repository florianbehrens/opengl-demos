#include "fbotexturenode.h"
#include "instancedhistogramscene.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>

FboTextureNode::FboTextureNode( QQuickWindow* parent )
    : QObject(),
      m_window( parent ),
      m_fbo( 0 ),
      m_texture( 0 ),
      m_scene( 0 )
{
    connect( m_window, SIGNAL( beforeRendering() ),
             this, SLOT( renderToTexture() ) );

    // Start a timer. Would be better to get this from some global
    // application timer as this object will get re-created when
    // the QQ2 view is resized in this example and so the time will
    // get reset. But hey, it's an example! ;)
    m_time.start();
}

FboTextureNode::~FboTextureNode()
{
    delete m_fbo;
    delete m_texture;
}

void FboTextureNode::renderToTexture()
{
    if ( !m_scene )
        return;

    // Get the node's size so that we can resize FBO (and hence texture)
    // and OpenGL viewport accordingly
    QSize size = rect().size().toSize();

    if ( !m_fbo )
    {
        // Create an FBO with color and depth attachments
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment( QOpenGLFramebufferObject::Depth );
        m_fbo = new QOpenGLFramebufferObject( size, format );

        // Wrap the OpenGL texture in a QSGTexture object for use with the scene graph.
        // Then set the texture as a simple material on this node
        m_texture = m_window->createTextureFromId( m_fbo->texture(), size );
        setTexture( m_texture );
        //setTextureCoordinatesTransform(MirrorVertically);
    }

    // Set the FBO as the render target
    m_fbo->bind();

    // Update the viewport size and render the scene
    m_scene->resize( size.width(), size.height() );
    float t = m_time.elapsed() / 1000.0f;
    m_scene->update( t );
    m_scene->render();

    // Reset render target to the default framebuffer
    m_fbo->bindDefault();

    // Force a redraw of the QtQuick2 scene
    m_window->update();
}
