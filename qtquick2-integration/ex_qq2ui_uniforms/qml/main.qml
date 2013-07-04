
import QtQuick 2.0

Item {
    id: root

    Text {
        anchors.top : parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15

        text: "F1 - Show/Hide Settings"
        font.pixelSize: 15
        color: "white"
        style: Text.Outline
    }

    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter;
        y: -100

        text: "Hello OpenGL and QQuick2!"
        color: "white"
        font.pixelSize: 45
        style: Text.Outline
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

    Rectangle {
        id: settingsPanelBackground

        color: "black"
        opacity: 0.7
        radius: 20

        anchors.centerIn: settingsPanel

        height: settingsPanel.height + 50
        width: settingsPanel.width
    }

    Item {
        id: settingsPanel

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        height: {
            updatesCheckBox.height + 30 +
            frequencySlider.height + 20 +
            spatialFrequencySlider.height + 20 +
            scaleSlider.height + 20 +
            timeScaleSlider.height + 20 +
            redSlider.height + 20 +
            greenSlider.height + 20 +
            blueSlider.height + 20;
        }
        width: parent.width - 100

        CheckBox {
            id: updatesCheckBox
            anchors.left: parent.horizontalCenter
            anchors.top: parent.top
            height: 50
            onCheckedChanged: _scene.updatesEnabled = checked
        }

        Text {
            id: updatesLabel
            anchors.verticalCenter: updatesCheckBox.verticalCenter
            anchors.right: updatesCheckBox.left
            anchors.rightMargin: 10

            text: "Enable Updates:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: frequencySlider

            anchors.left: parent.horizontalCenter
            anchors.top: updatesCheckBox.bottom
            anchors.topMargin: 30
            width: 300
            height: 40

            minimum: 0.2
            maximum: 5.0

            onValueChanged: _scene.frequency = value
        }

        Text {
            id: frequencyLabel
            anchors.verticalCenter: frequencySlider.verticalCenter
            anchors.right: frequencySlider.left
            anchors.rightMargin: 10

            text: "Frequency:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: spatialFrequencySlider

            anchors.left: parent.horizontalCenter
            anchors.top: frequencySlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.5
            maximum: 3.0

            onValueChanged: _scene.spatialFrequency = value
        }

        Text {
            id: spatialFrequencyLabel
            anchors.verticalCenter: spatialFrequencySlider.verticalCenter
            anchors.right: spatialFrequencySlider.left
            anchors.rightMargin: 10

            text: "Spatial Frequency:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: scaleSlider

            anchors.left: parent.horizontalCenter
            anchors.top: spatialFrequencySlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.5
            maximum: 3.0

            onValueChanged: _scene.scale = value
        }

        Text {
            id: scaleLabel
            anchors.verticalCenter: scaleSlider.verticalCenter
            anchors.right: scaleSlider.left
            anchors.rightMargin: 10

            text: "Scale:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: timeScaleSlider

            anchors.left: parent.horizontalCenter
            anchors.top: scaleSlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.01
            maximum: 1.0

            onValueChanged: _scene.timeScale = value
        }

        Text {
            id: timeScaleLabel
            anchors.verticalCenter: timeScaleSlider.verticalCenter
            anchors.right: timeScaleSlider.left
            anchors.rightMargin: 10

            text: "Time Scale:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: redSlider

            anchors.left: parent.horizontalCenter
            anchors.top: timeScaleSlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.0
            maximum: 2.0

            onValueChanged: _scene.redBias = value
        }

        Text {
            id: redLabel
            anchors.verticalCenter: redSlider.verticalCenter
            anchors.right: redSlider.left
            anchors.rightMargin: 10

            text: "Red Bias:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: greenSlider

            anchors.left: parent.horizontalCenter
            anchors.top: redSlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.0
            maximum: 2.0

            onValueChanged: _scene.greenBias = value
        }

        Text {
            id: greenLabel
            anchors.verticalCenter: greenSlider.verticalCenter
            anchors.right: greenSlider.left
            anchors.rightMargin: 10

            text: "Green Bias:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
        }

        Slider {
            id: blueSlider

            anchors.left: parent.horizontalCenter
            anchors.top: greenSlider.bottom
            anchors.topMargin: 20
            width: 300
            height: 40

            minimum: 0.0
            maximum: 2.0

            onValueChanged: _scene.blueBias = value
        }

        Text {
            id: blueLabel
            anchors.verticalCenter: blueSlider.verticalCenter
            anchors.right: blueSlider.left
            anchors.rightMargin: 10

            text: "Blue Bias:"
            font.pixelSize: 30
            style: Text.Outline
            color: "white"
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

    Component.onCompleted: {
        updatesCheckBox.checked = _scene.updatesEnabled;
        frequencySlider.value = _scene.frequency;
        spatialFrequencySlider.value = _scene.spatialFrequency;
        scaleSlider.value = _scene.scale;
        timeScaleSlider.value = _scene.timeScale;
        redSlider.value = _scene.redBias;
        greenSlider.value = _scene.greenBias;
        blueSlider.value = _scene.blueBias;
    }
}
