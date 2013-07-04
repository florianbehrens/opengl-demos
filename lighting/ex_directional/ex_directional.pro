include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            directionalscene.cpp

HEADERS  += window.h \
            directionalscene.h

RESOURCES += ex_directional.qrc

OTHER_FILES += shaders/directional.vert \
               shaders/directional.frag

DEPENDPATH += .
