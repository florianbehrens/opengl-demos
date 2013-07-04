#include <QApplication>
#include <QFileDialog>
#include <QSurfaceFormat>
#include <QMessageBox>

#include "simpletexturescene.h"
#include "window.h"

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

    QString movieFile = QFileDialog::getOpenFileName(0,
                                                     "Open video file",
                                                     QString(),
                                                     "Video files (*.avi *.mp4 *.mov *.flv);;All files (*.*)");

    if (movieFile.isEmpty() || !QFile::exists(movieFile)) {
        QMessageBox::critical(0, "Unable to open video file", "You need to specify an existing video file");
        return 0;
    }

    Window w( format );
    w.setScene( new SimpleTextureScene(movieFile) );

    w.show();
    return a.exec();
}
