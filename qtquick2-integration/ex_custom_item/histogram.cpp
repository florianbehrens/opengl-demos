#include "histogram.h"
#include "fbotexturenode.h"
#include "instancedhistogramscene.h"

#include <qsgsimpletexturenode.h>
#include <QDebug>
#include <QOpenGLContext>

Histogram::Histogram( QQuickItem* parent )
    : QQuickItem( parent ),
      m_scene( 0 )
{
    setFlag( ItemHasContents, true );
}

Histogram::~Histogram()
{
    delete m_scene;
}

QSGNode* Histogram::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* )
{
    // Don't bother with resize and such, just recreate the node from scratch
    // when geometry changes.
    if ( oldNode )
        delete oldNode;

    // Create the scene we will render and allow it to initialise resources
    if ( !m_scene )
    {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        m_scene = new InstancedHistogramScene();
        m_scene->setContext( context );
        m_scene->initialise();
    }

    // Create a new FboTextureNode and set scene on it
    qDebug() << "Recreating FboTextureNode";
    FboTextureNode* node = new FboTextureNode( window() );
    node->setScene( m_scene );
    node->setRect( boundingRect() );

    return node;
}
