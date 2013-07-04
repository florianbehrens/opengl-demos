include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            televisionscene.cpp

HEADERS  += \
            televisionscene.h

RESOURCES += ex_television.qrc

OTHER_FILES += shaders/television.vert \
               shaders/television.frag \
               shaders/phong.vert \
               shaders/phong.frag
