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
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import acanthis 1.0

RowLayout {
    spacing: toolbar.spacing
    id: toolbarRow

    FileDialog {
        id: openDialog
        onAccepted: window.openDocument(openDialog.selectedFile)
    }

    ToolButton {
        id: newButton
        onClicked: window.newDocument()
        Layout.fillHeight: true
        contentItem: Text {
            text: "🗎"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }

    ToolButton {
        id: openButton
        onClicked: openDialog.open()
        Layout.fillHeight: true
        contentItem: Text {
            text: "📂"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }

    ToolButton {
        id: saveButton
        onClicked: {
            console.log("UNIMPLEMENTED: Save file " + graphDisplayModel.get(
                            graphListNameView.currentIndex).graphModelFile.getFilename(
                            ))
        }
        Layout.fillHeight: true
        contentItem: Text {
            text: "💾"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }

    ToolButton {
        id: rightButton
        contentItem: Text {
            text: "☰"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        onClicked: drawer.open()

        Layout.fillHeight: true
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }
    ToolButton {
        id: splitHorizontal
        contentItem: Text {
            text: "horizontal"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        onClicked: {
            graphFileView.currentItem.splitActiveView(Qt.Horizontal);
       }

        Layout.fillHeight: true
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }
    ToolButton {
        id: splitVertical
        contentItem: Text {
            text: "vertical"
            horizontalAlignment: Text.AlignHCenter
            color: Theme.toolbarButtonTextColour
        }
        onClicked: {
            graphFileView.currentItem.splitActiveView(Qt.Vertical);
       }

        Layout.fillHeight: true
        background: Rectangle {
            Layout.fillHeight: true
            implicitWidth: parent.height
            radius: Theme.tabButtonHoverRadius
            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
        }
    }
}
