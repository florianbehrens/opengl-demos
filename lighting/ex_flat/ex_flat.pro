include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            flatscene.cpp

HEADERS  += \
            flatscene.h

RESOURCES += ex_flat.qrc

OTHER_FILES += shaders/flat.vert \
               shaders/flat.frag
