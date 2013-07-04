include( ../../common/common.pri )

TEMPLATE = app

QT += opengl

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            skyboxscene.cpp

HEADERS  += window.h \
            skyboxscene.h

RESOURCES += ex_skybox.qrc

OTHER_FILES += shaders/skybox.vert \
               shaders/skybox.frag
