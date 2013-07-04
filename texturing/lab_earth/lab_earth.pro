include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            earthscene.cpp

HEADERS  += window.h \
            earthscene.h

RESOURCES += lab_earth.qrc

OTHER_FILES += shaders/earth.vert \
               shaders/earth.frag
