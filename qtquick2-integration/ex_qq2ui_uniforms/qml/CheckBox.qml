import QtQuick 2.0


Item {
    id: radioButton

    property bool checked: false

    width: radio.width
    height: radio.height

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
        opacity: 1.0 - scale / 1.3
        Behavior on scale {
            NumberAnimation {  duration: 300; easing.type: Easing.InOutQuad }
        }
    }

    Image {
        id: radio
        source: radioButton.checked ? "on.png" : "off.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                radioButton.checked = !radioButton.checked;
                glow.scale = 1;
                resetglow.running = true;
            }
        }
    }
}
