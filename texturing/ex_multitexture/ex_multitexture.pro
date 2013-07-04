include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            multitexturescene.cpp

HEADERS  += \
            multitexturescene.h

RESOURCES += ex_multitexture.qrc

OTHER_FILES += shaders/multitexture.vert \
               shaders/multitexture.frag
