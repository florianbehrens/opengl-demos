include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            quadtessellationscene.cpp

HEADERS  += \
            quadtessellationscene.h

RESOURCES += ex_quad_tessellation.qrc

OTHER_FILES += shaders/quadtessellation.vert \
               shaders/quadtessellation.geom \
               shaders/quadtessellation.frag \
    shaders/quadtessellation.tcs \
    shaders/quadtessellation.tes
