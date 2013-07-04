#include <QGuiApplication>
#include <QQuickView>
#include <qqml.h>

#include "window.h"
#include "histogram.h"

int main( int argc, char* argv[] )
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Histogram>( "OpenGLComponents", 1, 0, "Histogram" );

    Window view;
    view.setResizeMode( QQuickView::SizeRootObjectToView );
    view.setSource( QUrl( "qrc:///main.qml" ) );
    view.show();

    return app.exec();
}
