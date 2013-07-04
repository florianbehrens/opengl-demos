include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            geometryscene.cpp \
            cylinder.cpp

HEADERS  += window.h \
            geometryscene.h \
            cylinder.h

RESOURCES += lab_geometry.qrc

OTHER_FILES += shaders/phong.vert \
               shaders/phong.frag
