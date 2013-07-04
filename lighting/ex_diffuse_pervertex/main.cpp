#include <QGuiApplication>
#include <QSurfaceFormat>

#include "diffusescene.h"
#include "openglwindow.h"

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv );

    // Specify the format we wish to use
    QSurfaceFormat format;
    format.setMajorVersion( 3 );
#if !defined(Q_OS_MAC)
    format.setMinorVersion( 3 );
#else
    format.setMinorVersion( 2 );
#endif
    format.setDepthBufferSize( 24 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );

    OpenGLWindow w( format );
    w.setScene( new DiffuseScene );

    w.show();
    return a.exec();
}
