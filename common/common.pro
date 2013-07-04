# For QGLWidget::convertToGLFormat
QT += opengl openglextensions widgets

TEMPLATE = lib
CONFIG += staticlib
TARGET = opengl-common

HEADERS += \
           $$PWD/axisalignedboundingbox.h \
           $$PWD/abstractscene.h \
           $$PWD/camera.h \
           $$PWD/camera_p.h \
           $$PWD/cube.h \
           $$PWD/fullscreenquad.h \
           $$PWD/material.h \
           $$PWD/mesh.h \
           $$PWD/objloader.h \
           $$PWD/plane.h \
           $$PWD/sampler.h \
           $$PWD/skybox.h \
           $$PWD/texture.h \
           $$PWD/torus.h \
           $$PWD/sphere.h \
           $$PWD/kdgeometryshader.h \
           $$PWD/grid.h \
           $$PWD/cameracontroller.h \
           $$PWD/basicwindow.h \
           $$PWD/camerascene.h \
           $$PWD/openglwindow.h \
           $$PWD/opengldebugmessagemodel.h \
           $$PWD/opengldebugmessagewindow.h


SOURCES += \
           $$PWD/axisalignedboundingbox.cpp \
           $$PWD/abstractscene.cpp \
           $$PWD/camera.cpp \
           $$PWD/cube.cpp \
           $$PWD/fullscreenquad.cpp \
           $$PWD/material.cpp \
           $$PWD/mesh.cpp \
           $$PWD/objloader.cpp \
           $$PWD/plane.cpp \
           $$PWD/sampler.cpp \
           $$PWD/skybox.cpp \
           $$PWD/texture.cpp \
           $$PWD/torus.cpp \
           $$PWD/sphere.cpp \
           $$PWD/kdgeometryshader.cpp \
           $$PWD/grid.cpp \
           $$PWD/cameracontroller.cpp \
           $$PWD/basicwindow.cpp \
           $$PWD/camerascene.cpp \
           $$PWD/openglwindow.cpp \
           $$PWD/opengldebugmessagemodel.cpp \
           $$PWD/opengldebugmessagewindow.cpp

