include( ../../common/common.pri )

QT       += quick

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            window.cpp \
            basicusagescene.cpp

HEADERS  += window.h \
            basicusagescene.h

RESOURCES += ex_basic_integration.qrc

OTHER_FILES += shaders/basic.vert \
               shaders/basic.frag \
               shaders/basic_2_1.vert \
               shaders/basic_2_1.frag \
               qml/main.qml
