include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            toonscene.cpp

HEADERS  += \
            toonscene.h

RESOURCES += ex_toon.qrc

OTHER_FILES += shaders/toon.vert \
               shaders/toon.frag
