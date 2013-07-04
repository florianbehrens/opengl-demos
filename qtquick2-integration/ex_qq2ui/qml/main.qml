
import QtQuick 2.0

Item {
    id: root

    Text {
        text: "F1 - Show/Hide Settings"
        font.pixelSize: 15
        anchors.top : parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Text {
        id: text
        text: "Hello OpenGL and QQuick2!"
        anchors.horizontalCenter: parent.horizontalCenter;
        y: -100
        font.pixelSize: 45
    }

    SequentialAnimation {
        running: true
        loops: Animation.Infinite

        PauseAnimation {
            duration: 3000
        }

        ParallelAnimation {
            NumberAnimation {
                target: text
                property: "opacity"
                from: 0
                to: 1
                duration: 1
            }

            NumberAnimation {
                target: text
                property: "font.letterSpacing"
                from: 100
                to: 0
                duration: 1
            }
        }

        NumberAnimation {
            target: text
            property: "y"
            from: -100
            to: 300
            duration: 3000
        }

        ParallelAnimation {
            NumberAnimation {
                target: text
                property: "font.letterSpacing"
                from: 0
                to: 100
                duration: 1500
            }

            NumberAnimation {
                target: text
                property: "opacity"
                from: 1
                to: 0
                duration: 1500
            }
        }
    }

    Item {
        id: settingsPanel

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        height: checkBox.height + slider.height
        width: 500

        CheckBox {
            id: checkBox
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: - 90
            anchors.top: parent.top
            height: 50

            text: "Enable Rotation:"

            onChecked: _scene.toggleRotation()
        }

        SliderControl {
            id: slider

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 27
            anchors.top: checkBox.bottom
            anchors.topMargin: 20
            width: 500
            height: 50

            onValueChanged: _scene.rotationSpeed = value
        }
    }

    states: [
        State {
            name: "SettingsHidden"
            PropertyChanges { target: settingsPanel; anchors.bottomMargin: -40 - settingsPanel.height }
        },

        State {
            name: "SettingsVisible"
            PropertyChanges { target: settingsPanel; anchors.bottomMargin: 60 }
        }
    ]

    transitions: [
        Transition {
            from: "SettingsHidden"
            to: "SettingsVisible"
            PropertyAnimation {
                property: "anchors.bottomMargin"
                easing.type: Easing.OutBounce
                duration: 1500
            }
        },

        Transition {
            from: "SettingsVisible"
            to: "SettingsHidden"
            PropertyAnimation {
                property: "anchors.bottomMargin"
                easing.type: Easing.InBack
                duration: 1000
            }
        }
    ]

    state: "SettingsHidden"

    focus: true
    Keys.onPressed: {
        if (event.key == Qt.Key_F1) {
            console.log("Show/Hide settings");

            if ( state == "SettingsHidden" )
                state = "SettingsVisible"
            else
                state = "SettingsHidden"

            event.accepted = true;
        }
    }
}
