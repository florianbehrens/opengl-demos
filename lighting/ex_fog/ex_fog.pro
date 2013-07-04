include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            fogscene.cpp

HEADERS  += fogscene.h

RESOURCES += ex_fog.qrc

OTHER_FILES += shaders/fog.vert \
               shaders/fog.frag
