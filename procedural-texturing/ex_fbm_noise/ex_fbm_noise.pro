include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            noisescene.cpp

HEADERS  += window.h \
            noisescene.h

RESOURCES += ex_noise.qrc

OTHER_FILES += shaders/noise.vert \
               shaders/noise.frag
