#include "window.h"

#include "instancedhistogramscene.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( QWindow* parent )
    : QQuickView( parent )
{
    // The OpenGL scene we are using in this example requires an
    // OpenGL 3.3 context (or 3.2 plus an extension on OS X).
    // QQ2 cannot be used with the Core profile (yet) so we request
    // a compatibility profile.
    QSurfaceFormat format;
    format.setMajorVersion( 3 );
#if !defined(Q_OS_MAC)
    format.setMinorVersion( 3 );
#else
    format.setMinorVersion( 2 );
#endif
    format.setProfile( QSurfaceFormat::CompatibilityProfile );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    format.setSamples( 8 );
    setFormat( format );
}
