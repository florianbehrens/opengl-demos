include( ../../common/common.pri )

QT += opengl widgets

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            simpletexturescene.cpp

HEADERS  += window.h \
            simpletexturescene.h

RESOURCES += ex_yuv_to_rgb_ffmpeg.qrc

OTHER_FILES += shaders/simpletexture.vert \
               shaders/simpletexture.frag
