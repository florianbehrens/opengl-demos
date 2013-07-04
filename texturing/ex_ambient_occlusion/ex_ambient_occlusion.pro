include( ../../common/common.pri )

QT       += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            ambientocclusionscene.cpp

HEADERS  += \
            ambientocclusionscene.h

RESOURCES += ex_ambient_occlusion.qrc

OTHER_FILES += shaders/ambientocclusion.vert \
               shaders/ambientocclusion.frag
