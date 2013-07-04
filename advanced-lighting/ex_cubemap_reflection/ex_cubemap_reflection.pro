include( ../../common/common.pri )

TEMPLATE = app

QT += opengl

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            cubemapreflectionscene.cpp

HEADERS  += window.h \
            cubemapreflectionscene.h

RESOURCES += ex_cubemapreflection.qrc

OTHER_FILES += shaders/skyboxreflection.vert \
               shaders/skyboxreflection.frag \
               shaders/skybox.vert \
               shaders/skybox.frag

