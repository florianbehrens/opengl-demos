include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            instancedgeometryscene.cpp \

HEADERS  += window.h \
            instancedgeometryscene.h \

RESOURCES += ex_instanced_geometry.qrc

OTHER_FILES += shaders/instancedgeometry.vert \
               shaders/instancedgeometry.frag
