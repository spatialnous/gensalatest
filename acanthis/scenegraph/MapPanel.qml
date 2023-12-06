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
import QtQuick.Templates as T

import acanthis 1.0

Panel {
    id: mappnl
    objectName: "mapPanel"
    title: qsTr("Maps")

    background: Rectangle {
        color: Theme.panelColour
    }
    property GraphModel graphModel
    property ViewListModel viewsModel

    header: RowLayout {
        objectName: parent.objectName + "Header"
        spacing: 0

        Label {
            objectName: parent.objectName + "TitleLabel"
            text: parent.parent.title
            font.bold: true
            color: Theme.panelTextColour

            Layout.leftMargin: 16
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            id: settingsPopupToolButton
            objectName: parent.objectName + "SettingsToolButton"

            contentItem: Text {
                text: "⚙"
                horizontalAlignment: Text.AlignHCenter
                color: Theme.toolbarButtonTextColour
            }
            focusPolicy: Qt.NoFocus
            visible: settingsPopup

            Layout.preferredWidth: implicitHeight

            onClicked: settingsPopup.open()
            background: Rectangle {
                Layout.fillHeight: true
                implicitWidth: parent.height
                radius: Theme.tabButtonHoverRadius
                color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
            }
        }

        ToolButton {
            objectName: parent.objectName + "HideShowToolButton"
            contentItem: Text {
                text: expanded ? "⯇" : "⯆"
                horizontalAlignment: Text.AlignHCenter
                color: Theme.toolbarButtonTextColour
            }
            focusPolicy: Qt.NoFocus

            Layout.leftMargin: -8
            Layout.preferredWidth: implicitHeight

            onClicked: expanded = !expanded
            background: Rectangle {
                Layout.fillHeight: true
                implicitWidth: parent.height
                radius: Theme.tabButtonHoverRadius
                color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
            }
        }
    }
    contentItem: ListView {
        id: fileTreeViews
        model: viewsModel
        interactive: false
        snapMode: ListView.SnapOneItem
        cacheBuffer: 0
        highlightRangeMode: ListView.StrictlyEnforceRange
        clip: true

        function setCurrentIndex(newCurrentIndex) {
            positionViewAtIndex(newCurrentIndex, ListView.Beginning)
            currentIndex = newCurrentIndex;
        }

        delegate: Item {
            width: parent == null ? 0 : parent.width
            height: parent == null ? 0 : fileTreeViews.height
            property var graphViewModel: model.graphViewModel
            property var graphViewId: model.graphViewModel.id
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    text: {
                        return "View: " + index
                    }
                    color: Theme.toolbarButtonTextColour
                }

                MapTreeView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    graphViewModel: parent.parent.graphViewModel
                }
            }
        }
    }

    footer: RowLayout {
        visible: mappnl.expanded

        RowActionButton {
            objectName: "newMapButton"

            Layout.leftMargin: 6

            ToolTip.text: qsTr("Add a new layer")

            onClicked: {
                root.graphModel.addNewLayer()
                layerListView.positionViewAtIndex(layerListView.currentIndex,
                                                  ListView.Contain)
            }
        }

        RowActionButton {
            objectName: "moveLayerDownButton"
            text: "\uf107"
            font.family: "FontAwesome"
            enabled: graphModel
                     && graphModel.currentLayerIndex < graphModel.layerCount - 1

            ToolTip.text: qsTr("Move the current layer down")

            onClicked: graphModel.moveCurrentLayerDown()
        }

        RowActionButton {
            objectName: "moveLayerUpButton"
            text: "\uf106"
            font.family: "FontAwesome"
            enabled: graphModel && graphModel.currentLayerIndex > 0

            ToolTip.text: qsTr("Move the current layer up")

            onClicked: graphModel.moveCurrentLayerUp()
        }

        RowActionButton {
            objectName: "duplicateLayerButton"
            text: "\uf24d"
            font.family: "FontAwesome"
            enabled: graphModel && graphModel.currentLayerIndex >= 0
                     && graphModel.currentLayerIndex < graphModel.layerCount

            ToolTip.text: qsTr("Duplicate the current layer")

            onClicked: graphModel.duplicateCurrentLayer()
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowActionButton {
            objectName: "deleteLayerButton"
            text: "\uf1f8"
            font.family: "FontAwesome"
            enabled: true

            ToolTip.text: qsTr("Delete the current layer")

            onClicked: graphModel.deleteCurrentLayer()
        }
    }
}
