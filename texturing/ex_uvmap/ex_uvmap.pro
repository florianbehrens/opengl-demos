include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            uvmapscene.cpp

HEADERS  += window.h \
            uvmapscene.h

RESOURCES += ex_uvmap.qrc

OTHER_FILES += shaders/simpletexture.vert \
               shaders/simpletexture.frag
