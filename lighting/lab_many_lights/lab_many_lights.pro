include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            manylightsscene.cpp

HEADERS  += \
            manylightsscene.h

RESOURCES += lab_many_lights.qrc

OTHER_FILES += shaders/multiplelights.vert \
               shaders/multiplelights.frag
