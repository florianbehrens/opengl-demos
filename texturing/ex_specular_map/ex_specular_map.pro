include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            specularmapscene.cpp

HEADERS  += window.h \
            specularmapscene.h

RESOURCES += ex_specular_map.qrc

OTHER_FILES += shaders/specularmap.vert \
               shaders/specularmap.frag
