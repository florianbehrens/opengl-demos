
import QtQuick 2.0

Item {
    width: 800
    height: 600

    Text {
        id: text
        text: "Hello OpenGL and QQuick2!"
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top
        anchors.topMargin: 10
        font.pixelSize: 45
    }

    Rectangle {
        id: rectangle
        width: 100
        height: 100
        x: 100
        y: 100
        color: "steelblue"
    }
}
