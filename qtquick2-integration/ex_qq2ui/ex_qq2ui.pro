include( ../../common/common.pri )

QT       += quick

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            normalmapscene.cpp

HEADERS  += window.h \
            normalmapscene.h

RESOURCES += ex_qq2ui.qrc

OTHER_FILES += shaders/normalmap.vert \
               shaders/normalmap.frag \
               qml/main.qml \
               qml/Slider.qml \
               qml/CheckBox.qml \
               qml/SliderControl.qml \
    shaders/normalmap_2_1.vert \
    shaders/normalmap_2_1.frag
