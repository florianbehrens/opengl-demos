include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            solarsystemscene.cpp \
            orbitingbody.cpp

HEADERS  += window.h \
            solarsystemscene.h \
            orbitingbody.h

RESOURCES += lab_solar_system.qrc

OTHER_FILES += shaders/phong.vert \
               shaders/phong.frag \
               shaders/noise.vert \
               shaders/noise.frag
