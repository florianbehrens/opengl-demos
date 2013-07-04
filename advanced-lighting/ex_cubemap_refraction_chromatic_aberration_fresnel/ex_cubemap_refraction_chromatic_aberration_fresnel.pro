include( ../../common/common.pri )

TEMPLATE = app

QT += opengl

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            cubemaprefractionscene.cpp

HEADERS  += window.h \
            cubemaprefractionscene.h

RESOURCES += ex_cubemaprefraction.qrc

OTHER_FILES += shaders/skyboxrefraction.vert \
               shaders/skyboxrefraction.frag
