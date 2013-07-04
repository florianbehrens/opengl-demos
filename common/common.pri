# QtOpenGL is needed For QGLWidget::convertToGLFormat
# QtWidgets is needed for the debug log viewer
QT += opengl openglextensions widgets

win32 {
    build_pass {
        CONFIG(debug, debug|release) {
            LIBS += $$OUT_PWD/../../common/debug/opengl-common.lib
        } else {
            LIBS += $$OUT_PWD/../../common/release/opengl-common.lib
        }
    }
} else {
    LIBS += $$OUT_PWD/../../common/libopengl-common.a
}
