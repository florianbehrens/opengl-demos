#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>

#include "normalmapscene.h"
#include "window.h"

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv );

    Window* mainWindow = new Window();

    QObject* scene = mainWindow->scene();
    mainWindow->rootContext()->setContextProperty( "_scene", scene );

    mainWindow->setMinimumSize( QSize( 800, 600 ) );
    mainWindow->setResizeMode( QQuickView::SizeRootObjectToView );
    mainWindow->setSource( QUrl::fromLocalFile( "../ex_qq2ui/qml/main.qml" ) );
    mainWindow->show();

    return a.exec();
}
