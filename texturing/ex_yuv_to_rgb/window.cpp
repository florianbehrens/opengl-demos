#include "window.h"

#include "simpletexturescene.h"

#include <QKeyEvent>

Window::Window( const QSurfaceFormat& format, QScreen* screen )
    : OpenGLWindow( format, screen )
{
}

void Window::keyPressEvent( QKeyEvent* ev )
{
    SimpleTextureScene* scene = static_cast<SimpleTextureScene*>( this->scene() );
    switch ( ev->key() )
    {
        case Qt::Key_Space:
            scene->toggleAnimation();
            break;

        default:
            OpenGLWindow::keyPressEvent( ev );
    }
}
