include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            multiplelightsscene.cpp

HEADERS  += \
            multiplelightsscene.h

RESOURCES += ex_multiple_lights.qrc

OTHER_FILES += shaders/multiplelights.vert \
               shaders/multiplelights.frag
