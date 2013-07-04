#include <QApplication>
#include <QSurfaceFormat>

#include "transformscene.h"
#include "observerscene.h"
#include "openglwindow.h"
#include "toolbox.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );

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

    TransformScene* scene = new TransformScene;

    OpenGLWindow w( format );
    w.setScene( scene );
    w.setTitle("Scene");

    OpenGLWindow w2( format );
    w2.setTitle("Observer");

    ObserverScene* obs = new ObserverScene(scene);
    w2.setScene( obs );

    QWidget* toolbox = new Toolbox(scene, obs);



    w.show();
    w2.show();
    toolbox->show();

    return a.exec();
}
