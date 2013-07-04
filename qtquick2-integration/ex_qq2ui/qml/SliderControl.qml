import QtQuick 2.0

Item {
    property alias text: label.text
    property alias value: slider.value
    property alias minimum: slider.minimum
    property alias maximum: slider.maximum

    width: label.width + 10 + slider.width
    height: Math.max( label.height, slider.height )
    
    Text {
        id: label
        text: "Rotation Speed:"
        anchors.left: parent.left
        font.pixelSize: 30
    }
    
    Slider {
        id: slider
        anchors.right: parent.right
        anchors.left: label.right
        anchors.leftMargin: 10
        anchors.verticalCenter: label.verticalCenter
    }
}
