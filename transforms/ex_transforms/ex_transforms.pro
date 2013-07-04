include( ../../common/common.pri )

TEMPLATE = app
QT += widgets

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            transformscene.cpp \
            observerscene.cpp \
            toolbox.cpp

HEADERS  += \
            transformscene.h \
            observerscene.h \
            toolbox.h

FORMS += toolbox.ui

RESOURCES += ex_transforms.qrc

OTHER_FILES += shaders/diffuse.vert \
               shaders/diffuse.frag
