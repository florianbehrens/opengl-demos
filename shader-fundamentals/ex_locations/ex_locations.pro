include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            locationsscene.cpp \

HEADERS  += \
            locationsscene.h \

RESOURCES += ex_locations.qrc

OTHER_FILES += shaders/locations.vert \
               shaders/locations.frag
