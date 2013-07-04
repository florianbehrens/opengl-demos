
import QtQuick 2.0

Item {
    Text {
        id: text
        text: "Hello OpenGL and QQuick2!"
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top
        anchors.topMargin: 10
        font.pixelSize: 45
    }

    Rectangle {
        id: rect
        width: 100
        height: 100
        x: 100
        y: 100
        color: "steelblue"
        opacity: 0.6

        Behavior on x { SpringAnimation { spring: 10; damping: 0.2 } }
        Behavior on y { SpringAnimation { spring: 10; damping: 0.2 } }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            rect.x = mouse.x - rect.width/2
            rect.y = mouse.y - rect.height/2
        }
    }
}
