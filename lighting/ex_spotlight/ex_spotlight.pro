include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            spotlightscene.cpp

HEADERS  += window.h \
            spotlightscene.h

RESOURCES += ex_spotlight.qrc

OTHER_FILES += shaders/spotlight.vert \
               shaders/spotlight.frag

DEPENDPATH += .
