#include <QtWidgets/QApplication>
#include <QtQuick/QQuickView>
#include "window.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );

    Window* mainWindow = new Window();
    mainWindow->setMinimumSize( QSize( 800, 600 ) );
    mainWindow->setResizeMode( QQuickView::SizeRootObjectToView );
    mainWindow->setSource( QUrl::fromLocalFile( "../ex_updates/qml/main.qml" ) );
    mainWindow->show();

    return a.exec();
}
