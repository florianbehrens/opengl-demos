include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            phongscene.cpp

HEADERS  += \
            phongscene.h

RESOURCES += ex_normals_tangents.qrc

OTHER_FILES += shaders/phong.vert \
               shaders/phong.frag \
               shaders/line.vert \
               shaders/line.frag \

