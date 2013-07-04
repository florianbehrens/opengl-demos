include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            normalmapscene.cpp

HEADERS  += window.h \
            normalmapscene.h

RESOURCES += ex_normal_map.qrc

OTHER_FILES += shaders/normalmap.vert \
               shaders/normalmap.frag
