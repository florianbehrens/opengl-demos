#include <QDebug>
#include <QGuiApplication>
#include <QSurfaceFormat>

#include "edgedetectionscene.h"
#include "openglwindow.h"

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv );

    QStringList args = a.arguments();
    if ( args.size() != 2 )
    {
        qDebug() << "Please specify an obj file to load";
        exit( 1 );
    }
    const QString fileName = args.last();

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
    w.setScene( new EdgeDetectionScene( fileName ) );

    w.show();
    return a.exec();
}
