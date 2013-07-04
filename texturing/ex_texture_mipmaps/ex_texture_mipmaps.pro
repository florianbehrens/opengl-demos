include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            texturescene.cpp

HEADERS  += window.h \
            texturescene.h

RESOURCES += ex_texture.qrc

OTHER_FILES += shaders/texture.vert \
               shaders/texture.frag
