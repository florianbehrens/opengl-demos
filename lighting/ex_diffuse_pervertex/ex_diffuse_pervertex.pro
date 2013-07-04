include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            diffusescene.cpp

HEADERS  += \
            diffusescene.h

RESOURCES += ex_diffuse.qrc

OTHER_FILES += shaders/diffuse.vert \
               shaders/diffuse.frag
