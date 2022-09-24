// Copyright (C) 2021 - 2022 Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
    property GraphDocument graphDocument

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

        onClicked: {
            model.layer.visible = !model.layer.visible
            gl_map_view.update()
        }
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
            graphDocument.setLayerName(index, text)
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
