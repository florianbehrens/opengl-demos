include( ../../common/common.pri )

QT       += quick openglextensions

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            instancedhistogramscene.cpp

HEADERS  += window.h \
            instancedhistogramscene.h

RESOURCES += ex_qq2ui_uniforms.qrc

OTHER_FILES += shaders/instancedhistogram.vert \
               shaders/instancedhistogram.frag \
               shaders/noise.vert \
               shaders/noise.frag \
               qml/main.qml \
               qml/Slider.qml \
               qml/CheckBox.qml \
    shaders/instancedhistogram_2_1.vert \
    shaders/instancedhistogram_2_1.frag \
    shaders/noise_2_1.vert \
    shaders/noise_2_1.frag
