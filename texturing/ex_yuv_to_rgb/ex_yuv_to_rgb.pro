include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            simpletexturescene.cpp

HEADERS  += window.h \
            simpletexturescene.h

RESOURCES += ex_yuv_to_rgb.qrc

OTHER_FILES += shaders/simpletexture.vert \
               shaders/simpletexture.frag
