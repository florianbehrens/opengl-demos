include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            phongscene.cpp

HEADERS  += \
            phongscene.h

RESOURCES += lab_phong.qrc

OTHER_FILES += shaders/phong.vert \
               shaders/phong.frag
