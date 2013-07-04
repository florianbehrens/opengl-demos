include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            proceduralscene.cpp

HEADERS  += window.h \
            proceduralscene.h

RESOURCES += ex_procedural.qrc

OTHER_FILES += shaders/procedural.vert \
               shaders/procedural.frag
