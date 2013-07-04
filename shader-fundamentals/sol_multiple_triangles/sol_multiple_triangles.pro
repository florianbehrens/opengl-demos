include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            multipletrianglesscene.cpp \

HEADERS  += multipletrianglesscene.h \

RESOURCES += lab_multiple_triangles.qrc

OTHER_FILES += shaders/basic.vert \
               shaders/basic.frag
