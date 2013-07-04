include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            basicusagescene.cpp \

HEADERS  += basicusagescene.h \

RESOURCES += ex_glsl.qrc

OTHER_FILES += shaders/hsv.vert \
               shaders/hsv.frag
