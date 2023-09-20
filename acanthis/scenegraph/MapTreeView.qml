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
    id: mapLayersTree
    property var graphViewModel

    ScrollBar.vertical: ScrollBar {}

    model: AQMapViewModel {
        id: memodl
        graphViewModel: mapLayersTree.graphViewModel
        Component.onCompleted: {
            memodl.resetItems()
        }
    }

    delegate: Item {
        id: root

        function getImplicitWidth() {
            switch (model.column) {
            case 0: // visible
                // first column, also contains the tree arrows
                return padding + visibilityCheckbox.implicitWidth;
            case 1: // editable
            default:
                return padding + visibilityCheckbox.implicitWidth;
            case nbuttons.length:
                // label
                return label.implicitWidth + padding;
            }
        }

        implicitWidth: getImplicitWidth()
        implicitHeight: visibilityCheckbox.implicitHeight * 1.5

        readonly property real indent: 10
        readonly property real padding: 5

        required property TreeView treeView
        required property bool isTreeNode
        required property bool expanded
        required property int hasChildren
        required property int depth

        readonly property int nbuttons: 2

        Text {
            id: visibilityCheckbox
            visible: model.column === 0
            text: model.visibility ? "\uf06e" : "\uf070"
            color: Theme.toolbarButtonTextColour
            TapHandler {
                onTapped: {
                    memodl.setItemVisible(memodl.index(model.row, model.column),
                                          !model.visibility)
                    graphViews.redraw();
                }
            }
        }
        Text {
            visible: model.column === 1
            text: model.editability ? "\uf040" : "\uf023"
            color: Theme.toolbarButtonTextColour
            TapHandler {
                onTapped: {
                    memodl.setItemEditable(memodl.index(model.row, model.column),
                                           !model.editability)
                    graphViews.redraw();
                }
            }
        }
        Text {
            x: (root.depth * root.indent)
            visible: root.hasChildren && model.column === nbuttons
            text: root.expanded ? "▼" : "▶"
            color: Theme.toolbarButtonTextColour
            TapHandler {
                onTapped: {
                    treeView.toggleExpanded(row)
                }
            }
        }
        Text {
            id: label
            Layout.fillWidth: true
            // The actual text is properly placed at the end of all icons (except the arrow).
            // Only push out the width of the arrow
            x: (visibilityCheckbox.width + padding) + (root.depth * root.indent)
            visible: model.column === nbuttons
            clip: true
            text: model.name
            color: Theme.toolbarButtonTextColour
            horizontalAlignment: Text.AlignLeft
        }
    }
}
