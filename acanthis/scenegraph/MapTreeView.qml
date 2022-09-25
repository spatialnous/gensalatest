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
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window

import acanthis 1.0

TreeView {
    visible: true
    clip: true

    Layout.fillWidth: true
    Layout.fillHeight: true

    ScrollBar.vertical: ScrollBar {}

    model: MapModel {
        id: memodl
        graphDocument: root.graphDocument
        Component.onCompleted: {
            memodl.resetItems()
        }
    }

    delegate: Item {
        id: root

        implicitWidth: padding + label.x + label.implicitWidth + padding
        implicitHeight: label.implicitHeight * 1.5

        readonly property real indent: 20
        readonly property real padding: 5

        required property TreeView treeView
        required property bool isTreeNode
        required property bool expanded
        required property int hasChildren
        required property int depth

        TapHandler {
            onTapped: treeView.toggleExpanded(row)
        }
        Text {
            id: label
            anchors.left: visibilityCheckBox.right
            anchors.verticalCenter: parent.verticalCenter
            x: padding + (root.isTreeNode ? (root.depth + 1) * root.indent : 0)
            width: root.width - root.padding - x
            clip: true
            text: model.treeitem.name
            color: Theme.toolbarButtonTextColour
        }
        CheckBox {
            id: visibilityCheckBox
            x: 14
            font.family: "FontAwesome"
            focusPolicy: Qt.NoFocus
            indicator: null
            anchors.verticalCenter: parent.verticalCenter
            contentItem: Text {
                text: model.treeitem.visible ? "\uf06e" : "\uf070"
                color: Theme.toolbarButtonTextColour
            }

            onClicked: {
                model.treeitem.visible = !model.treeitem.visible
                gl_map_view.update()
            }
        }
    }
}
