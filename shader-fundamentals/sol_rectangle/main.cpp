#include <QGuiApplication>
#include <QSurfaceFormat>

#include "basicusagescene.h"
#include "basicwindow.h"

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
    format.setProfile( QSurfaceFormat::CoreProfile );

    BasicOpenGLWindow w( format );
    w.setScene( new BasicUsageScene );

    w.show();
    return a.exec();
}
