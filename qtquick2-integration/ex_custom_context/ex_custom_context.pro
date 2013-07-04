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
               qml/main.qml
