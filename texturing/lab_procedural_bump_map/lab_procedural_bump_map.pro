include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            bumpscene.cpp

HEADERS  += window.h \
            bumpscene.h

RESOURCES += lab_procedural_bump_map.qrc

OTHER_FILES += shaders/bumpmap.vert \
               shaders/bumpmap.frag \
               shaders/line.vert \
               shaders/line.frag

