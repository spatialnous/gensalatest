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
    id: control

    ScrollBar.vertical: ScrollBar {}

    model: MapModel {
        id: memodl
        graphDocument: mappnl.graphDocument
        Component.onCompleted: {
            memodl.resetItems()
        }
    }

    delegate: Item {
        id: root

        implicitWidth: model.column === 0 ? padding + visibilityCheckBox.implicitWidth : label.implicitWidth + padding
        implicitHeight: visibilityCheckBox.implicitHeight * 1.5

        readonly property real indent: 10
        readonly property real padding: 5

        required property TreeView treeView
        required property bool isTreeNode
        required property bool expanded
        required property int hasChildren
        required property int depth

        Text {
            id: visibilityCheckBox
            visible: model.column === 0
            text: model.visibility ? "\uf06e" : "\uf070"
            color: Theme.toolbarButtonTextColour
            TapHandler {
                onTapped: {
                    memodl.setItemVisibility(memodl.index(model.row,
                                                          model.column),
                                             !model.visibility)
                    graphViews.redraw();
                }
            }
        }
        Rectangle {
            id: label
            x: visibilityCheckBox.width + padding + (root.depth * root.indent)
            Layout.alignment: Qt.AlignLeft
            Text {
                visible: root.isTreeNode && root.hasChildren
                         && model.column === 0
                text: root.expanded ? "▼" : "▶"
                color: Theme.toolbarButtonTextColour
                TapHandler {
                    onTapped: {
                        treeView.toggleExpanded(row)
                    }
                }
            }
            Text {
                Layout.fillWidth: true
                visible: model.column === 1
                clip: true
                text: model.name
                color: Theme.toolbarButtonTextColour
                horizontalAlignment: Text.AlignLeft
            }
        }
    }
}
