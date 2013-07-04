import QtQuick 2.0

Item {
    id: root

    property alias text: textItem.text

    signal checked(bool checkValue)

    width: textItem.width + radioButton.width
    height: Math.max( textItem.height, radioButton.height )

    Text {
        id: textItem
        anchors.left: parent.left
        anchors.verticalCenter: radioButton.verticalCenter
        text: "Option"
        font.pixelSize: 30//checkImage.height - 4
    }

    Item {
        id: radioButton
        width: radio.width
        height: radio.height
        anchors.left: textItem.right
        anchors.leftMargin: 10
        anchors.verticalCenter: textItem.verticalCenter

        property bool on: false

        Timer {
            id:resetglow
            interval: 400
            onTriggered: glow.scale=0.4
        }

        Image {
            id: glow
            source:radioButton.on ? "onglow.png" : "offglow.png"
            anchors.horizontalCenter: radio.horizontalCenter
            anchors.verticalCenter: radio.verticalCenter
            scale: 0.4
            opacity: 1 - scale / 1.3
            Behavior on scale {
                NumberAnimation {  duration: 300; easing.type: Easing.InOutQuad }
            }
        }

        Image {
            id: radio
            source: radioButton.on ? "on.png" : "off.png"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    radioButton.on = radioButton.on ? false : true;
                    glow.scale = 1;
                    resetglow.running = true;

                    // Emit signal
                    root.checked( radioButton.on );
                }
            }
        }
    }
}
