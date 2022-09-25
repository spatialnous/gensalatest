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

import acanthis 1.0

import "." as Ui

Panel {
    id: root
    objectName: "mapPanel"
    title: qsTr("Maps")
    padding: 0

    background: Rectangle {
        color: Theme.panelColour
    }

    property GraphDocument graphDocument

    readonly property int minimumUsefulHeight: header.implicitHeight
    // Estimate delegate height since we can't easily know what it is for all styles.
                                               + 48 + footer.implicitHeight

    //    UiStateSerialisation {
    //        project: root.project
    //        onReadyToLoad: {
    //            root.expanded = root.project.uiState.value("layerPanelExpanded", true)
    //            layerListView.contentY = root.project.uiState.value("layerListViewContentY", 0)
    //        }
    //        onReadyToSave: {
    //            root.project.uiState.setValue("layerPanelExpanded", root.expanded)
    //            root.project.uiState.setValue("layerListViewContentY", layerListView.contentY)
    //        }
    //    }

    //    ButtonGroup {
    //        objectName: "layerPanelButtonGroup"
    //        buttons: layerListView.contentItem.children
    //    }
    contentItem: ColumnLayout {
        visible: true
        spacing: 0
        width: 600
        height: 400

        MapTreeView {}
    }

    footer: RowLayout {
        visible: root.expanded

        RowActionButton {
            objectName: "newMapButton"

            Layout.leftMargin: 6

            ToolTip.text: qsTr("Add a new layer")

            onClicked: {
                root.graphDocument.addNewLayer()
                layerListView.positionViewAtIndex(layerListView.currentIndex,
                                                  ListView.Contain)
            }
        }

        RowActionButton {
            objectName: "moveLayerDownButton"
            text: "\uf107"
            font.family: "FontAwesome"
            enabled: graphDocument
                     && graphDocument.currentLayerIndex < graphDocument.layerCount - 1

            ToolTip.text: qsTr("Move the current layer down")

            onClicked: graphDocument.moveCurrentLayerDown()
        }

        RowActionButton {
            objectName: "moveLayerUpButton"
            text: "\uf106"
            font.family: "FontAwesome"
            enabled: graphDocument && graphDocument.currentLayerIndex > 0

            ToolTip.text: qsTr("Move the current layer up")

            onClicked: graphDocument.moveCurrentLayerUp()
        }

        RowActionButton {
            objectName: "duplicateLayerButton"
            text: "\uf24d"
            font.family: "FontAwesome"
            enabled: graphDocument && graphDocument.currentLayerIndex >= 0
                     && graphDocument.currentLayerIndex < graphDocument.layerCount

            ToolTip.text: qsTr("Duplicate the current layer")

            onClicked: graphDocument.duplicateCurrentLayer()
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

            //enabled: project && project.currentLayer && project.layerCount > 1
            ToolTip.text: qsTr("Delete the current layer")

            onClicked: graphDocument.deleteCurrentLayer()
        }
    }
}
