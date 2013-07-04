include( ../../common/common.pri )

QT += opengl openglextensions qml quick

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            instancedhistogramscene.cpp \
            histogram.cpp \
            fbotexturenode.cpp

HEADERS  += window.h \
            instancedhistogramscene.h \
            histogram.h \
            fbotexturenode.h

RESOURCES += ex_custom_item.qrc

OTHER_FILES += shaders/instancedhistogram.vert \
               shaders/instancedhistogram.frag \
               shaders/instancedhistogram_2_1.vert \
               shaders/instancedhistogram_2_1.frag \
               shaders/noise.vert \
               shaders/noise.frag \
               shaders/noise_2_1.vert \
               shaders/noise_2_1.frag \
               main.qml
