include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            antialiasingscene.cpp

HEADERS  += window.h \
            antialiasingscene.h

RESOURCES += ex_antialiasing.qrc

OTHER_FILES += shaders/antialiasing.vert \
               shaders/antialiasing.frag
