import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import acanthis 1.0

import "." as Ui

ItemDelegate {
    objectName: model.layer.name
    checkable: true
    checked: true
    focusPolicy: Qt.NoFocus

    CheckBox {
        id: visibilityCheckBox
        objectName: "layerVisibilityCheckBox"
        x: 14
        font.family: "FontAwesome"
        focusPolicy: Qt.NoFocus
        indicator: null
        anchors.verticalCenter: parent.verticalCenter
        contentItem: Text {
            text: model.layer.visible ? "\uf06e" : "\uf070"
            color: Theme.toolbarButtonTextColour
        }

        //        onClicked: project.setLayerVisible(index, !model.layer.visible)
    }

    TextField {
        id: layerNameTextField
        objectName: "layerNameTextField"
        text: model.layer.name
        activeFocusOnPress: false
        anchors.left: visibilityCheckBox.right
        anchors.leftMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: parent.rightPadding
        anchors.verticalCenterOffset: 6
        background.visible: false
        font.pixelSize: 12
        visible: false
        color: Theme.toolbarButtonTextColour

        onAccepted: {
            project.setLayerName(index, text)
            layeredImageCanvas.forceActiveFocus()
        }

        Keys.onEscapePressed: {
            text = model.layer.name
            layeredImageCanvas.forceActiveFocus()
        }
    }

    // We don't want TextField's editable cursor to be visible,
    // so we set visible: false to disable the cursor, and instead
    // render it via this.
    ShaderEffectSource {
        sourceItem: layerNameTextField
        anchors.fill: layerNameTextField
    }

    // Apparently the one above only works for the top level control item,
    // so we also need one for the background.
    ShaderEffectSource {
        sourceItem: layerNameTextField.background
        x: layerNameTextField.x + layerNameTextField.background.x
        y: layerNameTextField.y + layerNameTextField.background.y
        width: layerNameTextField.background.width
        height: layerNameTextField.background.height
        visible: layerNameTextField.activeFocus
    }

    Rectangle {
        id: focusRect
        width: 2
        height: parent.height
        color: Ui.Theme.focusColour
        visible: parent.checked
    }
}
