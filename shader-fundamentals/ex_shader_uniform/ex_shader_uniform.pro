include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            uniformscene.cpp \

HEADERS  += uniformscene.h \

RESOURCES += ex_shader_uniform.qrc

OTHER_FILES += shaders/basic_uniform.vert \
               shaders/basic_uniform.frag