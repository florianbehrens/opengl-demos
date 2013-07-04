include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            terraintessellationscene.cpp

HEADERS  += window.h \
            terraintessellationscene.h

RESOURCES += ex_terrain_tessellation.qrc

OTHER_FILES += shaders/terraintessellation.vert \
               shaders/terraintessellation.geom \
               shaders/terraintessellation.frag \
               shaders/terraintessellation.tcs \
               shaders/terraintessellation.tes
