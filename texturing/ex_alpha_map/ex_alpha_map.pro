include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            alphamapscene.cpp

HEADERS  += window.h \
            alphamapscene.h

RESOURCES += ex_alpha_map.qrc

OTHER_FILES += shaders/alphamap.vert \
               shaders/alphamap.frag
