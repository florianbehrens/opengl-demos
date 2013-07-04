include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            manyobjectsscene.cpp

HEADERS  += \
            manyobjectsscene.h

RESOURCES += ex_vaos.qrc

OTHER_FILES += shaders/basic.vert \
               shaders/basic.frag
