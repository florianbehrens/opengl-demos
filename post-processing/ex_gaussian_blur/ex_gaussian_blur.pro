include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            gaussianblurscene.cpp

HEADERS  += \
            gaussianblurscene.h

RESOURCES += ex_gaussian_blur.qrc

OTHER_FILES += shaders/gaussianblur.vert \
               shaders/gaussianblur.frag \
               shaders/phong.vert \
               shaders/phong.frag
