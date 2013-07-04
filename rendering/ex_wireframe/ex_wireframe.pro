include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            wireframescene.cpp

HEADERS  += window.h \
            wireframescene.h

RESOURCES += ex_wireframe.qrc

OTHER_FILES += shaders/wireframe.vert \
               shaders/wireframe.geom \
               shaders/wireframe.frag \
    shaders/wireframe.geom
