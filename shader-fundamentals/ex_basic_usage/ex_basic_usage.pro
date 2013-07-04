include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            basicusagescene.cpp \

HEADERS  += basicusagescene.h \

RESOURCES += ex_shader_basic_usage.qrc

OTHER_FILES += shaders/basic.vert \
               shaders/basic.frag
