include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            simpletexturescene.cpp

HEADERS  += window.h \
            simpletexturescene.h

RESOURCES += ex_simpletexture.qrc

OTHER_FILES += shaders/simpletexture.vert \
               shaders/simpletexture.frag
