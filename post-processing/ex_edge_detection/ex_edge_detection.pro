include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            edgedetectionscene.cpp

HEADERS  += \
            edgedetectionscene.h

RESOURCES += ex_edge_detection.qrc

OTHER_FILES += shaders/edgedetect.vert \
               shaders/edgedetect.frag \
               shaders/phong.vert \
               shaders/phong.frag
