#include <QGuiApplication>
#include <QSurfaceFormat>

#include "quadtessellationscene.h"
#include "openglwindow.h"

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv );

    // Specify the format we wish to use
    QSurfaceFormat format;
    format.setMajorVersion( 4 );
#if !defined(Q_OS_MAC)
    format.setMinorVersion( 0 );
#else
    format.setMinorVersion( 0 );
#endif
    format.setDepthBufferSize( 24 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );

    OpenGLWindow w( format );
    w.setScene( new QuadTessellationScene );

    w.show();
    return a.exec();
}
