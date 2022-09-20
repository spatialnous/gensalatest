// Copyright (C) 2021 Petros Koutsolampros

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

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.settings 1.0
import QtQuick.Dialogs 1.2
import acanthis 1.0

import "." as Ui

ApplicationWindow {
    id: window
    flags: Qt.FramelessWindowHint
    property string appName: "acanthis"

    width: 800
    height: 600

    visible: true

    Settings {
        id: settings
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height

        property color glViewForegroundColour: Qt.rgba(0, 0, 0, 255)
        property color glViewBackgroundColour: Qt.rgba(255, 255, 255, 255)
        property int glViewAntialiasingSamples: 0
        property bool glViewHighlightOnHover: true
    }

    ListModel {
        id: graphDisplayModel
    }

    FileDialog {
        id: openDialog
        onAccepted: {
            documentManager.openDocument(openDialog.fileUrl)
            let lastDocumentIndex = documentManager.lastDocumentIndex();
            console.log(lastDocumentIndex + " " + documentManager.numOpenedDocuments() - 1)
            if(lastDocumentIndex === documentManager.numOpenedDocuments() - 1) {
                graphDisplayModel.append({graphDocumentFile: documentManager.lastDocument, current: true})
            }
            graphListNameView.currentIndex = lastDocumentIndex;

        }
    }

    header: ToolBar {
        id: toolbar
        background: Rectangle {
            color: Theme.panelColour
        }
        spacing: 3


        ColumnLayout {
            anchors.fill: parent
            spacing: toolbar.spacing


            RowLayout {
                spacing: toolbar.spacing
                id: titlebarRow

                Layout.leftMargin: toolbar.spacing
                Layout.topMargin: toolbar.spacing
                Layout.rightMargin: toolbar.spacing
                Layout.bottomMargin: 0


                ButtonGroup {
                    id: btnGrp
                }

                ListView {
                    id: graphListNameView
                    model: graphDisplayModel
                    spacing: toolbar.spacing
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: (parent.height * 2 * graphDisplayModel.count)
                    orientation: ListView.Horizontal

                    delegate: Button {
                        id: tabButton
                        anchors.verticalCenter: parent == null ? window.verticalCenter : parent.verticalCenter
                        ButtonGroup.group: btnGrp
                        width: (graphListNameView.width - (graphListNameView.spacing * (graphDisplayModel.count +1))) / graphDisplayModel.count
                        height: parent == null ? 0 : parent.height
                        padding: 0
                        onClicked: {
                            graphListNameView.currentIndex = index;
                        }
                        ToolTip.visible: hovered
                        ToolTip.delay: Theme.tooltipDelay
                        ToolTip.text: graphDocumentFile.getFilename()

                        background: Rectangle {
                            color: {
                                if (hovered) {
                                    graphListNameView.currentIndex === index ? Theme.activeTabHoverColour : Theme.inactiveTabHoverColour
                                } else {
                                    graphListNameView.currentIndex === index ? Theme.activeTabColour : Theme.inactiveTabColour
                                }
                            }
                            radius: {
                                hovered || graphListNameView.currentIndex === index ? Theme.tabButtonHoverRadius : 0
                            }
                        }
                        contentItem: RowLayout {
                            spacing: 0
                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: graphDocumentFile.getFilename()
                                elide: Text.ElideLeft
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                                color: Theme.toolbarButtonTextColour

                                // in the case of a long (and thus truncated) path, this pushes the
                                // prefixed ellipsis further to the right
                                leftPadding: 5
                            }
                            Button {
                                id: tabCloseButton
                                contentItem: Text {
                                    text: "✕"
                                    horizontalAlignment: Text.AlignHCenter
                                    color: Theme.toolbarButtonTextColour
                                }
                                Layout.alignment: Qt.AlignCenter
                                background: Rectangle {
                                    width: 21
                                    height: 21
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    radius: tabCloseButton.width * 0.5
                                    color: tabCloseButton.hovered ? Theme.tabCloseButtonHoverColour : Theme.tabCloseButtonColour
                                }
                                onClicked: {
                                    documentManager.removeDocument(index);
                                    graphDisplayModel.remove(index)
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    // provides an area next to the tabs that allows for
                    // dragging the whole window (since it has no titlebar)
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: parent.height * 2
                    color: Theme.panelColour
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: graphDisplayModel.count === 0 ? window.appName : ""
                        color: Theme.appNameColour
                        font.bold: true
                    }

                    MouseArea {
                        id: iMouseArea
                        property int prevX: 0
                        property int prevY: 0
                        anchors.fill: parent
                        onPressed: {prevX=mouse.x; prevY=mouse.y}
                        onPositionChanged:{
                            var deltaX = mouse.x - prevX;
                            window.x += deltaX;
                            prevX = mouse.x - deltaX;

                            var deltaY = mouse.y - prevY
                            window.y += deltaY;
                            prevY = mouse.y - deltaY;
                        }
                    }
                }

                Button {
                    id: appCloseButton
                    contentItem: Text {
                        text: "✕"
                        horizontalAlignment: Text.AlignHCenter
                        color: Theme.toolbarButtonTextColour
                    }
                    Layout.alignment: Qt.AlignCenter
                    background: Rectangle {
                        width: 21
                        height: 21
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        radius: appCloseButton.width * 0.5
                        color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
                    }
                    onClicked: {
                        window.close()
                    }
                }
            }


            RowLayout {
                spacing: toolbar.spacing
                id: toolbarRow

                Layout.leftMargin: toolbar.spacing
                Layout.topMargin: 0
                Layout.rightMargin: toolbar.spacing
                Layout.bottomMargin: toolbar.spacing

                ToolButton {
                    id: newButton
                    onClicked: {
                        documentManager.createEmptyDocument()
                        graphDisplayModel.append({graphDocumentFile: documentManager.lastDocument, current: true})
                        graphListNameView.currentIndex = documentManager.lastDocumentIndex();
                    }
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
                                        graphListNameView.currentIndex).graphDocumentFile.getFilename())
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
            }
        }
    }

    ListView {
        // Instead of displaying one item at a time we load every
        // file's View (GL, Layers etc.) and only display the one
        // that is the current index.
        id: graphListGLView
        model: graphDisplayModel
        anchors.fill: parent
        interactive: false
        snapMode: ListView.SnapOneItem

        delegate: SplitView {
            id: mainSplitView
            objectName: "mainSplitView"
            anchors.top: parent == null ? window.top : parent.top
            width: parent == null ? 0 : parent.width
            height: graphListGLView.height
            orientation: Qt.Horizontal
            handle: Item {
                implicitWidth: 4
            }
            visible: graphListNameView.currentIndex == index
            Layout.fillWidth: true

            SplitView {
                id: graphView
                SplitView.fillWidth: true
                focus: true

                GLMapView {
                    id: gl_map_view
                    graphDocument: graphDocumentFile
                    foregroundColour: settings.glViewForegroundColour
                    backgroundColour: settings.glViewBackgroundColour
                    antialiasingSamples: 0
                    highlightOnHover: true
                    //anchors.fill: parent

                    // it is necessary to "flip" the FBO here because the default assumes
                    // that y is already flipped. Instead this will be handled internally
                    // to be in line with depthmapX
                    mirrorVertically: true

                    SplitView.preferredWidth: window.width / 3
                    anchors.fill: parent
                    focus: true
                }
            }

            SplitView {
                id: panelSplitView
                objectName: "panelSplitView"
                orientation: Qt.Vertical
                handle: Item {
                    implicitHeight: 4
                }

                readonly property int defaultPreferredWidth: 240

                SplitView.minimumWidth: 200
                SplitView.preferredWidth: defaultPreferredWidth
                SplitView.maximumWidth: window.width / 3

                Loader {
                    objectName: "mapsLoader"
                    //active: window.isLayeredImageProjectType && window.canvas
                    active: true
                    visible: active
                    sourceComponent: Ui.MapPanel {
                        //                        layeredImageCanvas: window.canvas
                        project: graphDocumentFile
                        //z: canvasContainer.z - 1
                    }

                    SplitView.minimumHeight: active
                                             && item.expanded ? item.minimumUsefulHeight : undefined
                    SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
                    SplitView.fillHeight: active && item.expanded
                }

                Loader {
                    objectName: "tilesetSwatchLoader"
                    //active: window.projectType === Project.TilesetType && window.canvas
                    active: true
                    visible: active
                    sourceComponent: Ui.AttributePanel {
                        //                        layeredImageCanvas: window.canvas
                        project: graphDocumentFile
                        //z: canvasContainer.z - 1
                    }

                    SplitView.minimumHeight: active && item.expanded ? item.header.implicitHeight : undefined
                    SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
                    SplitView.fillHeight: active && item.expanded
                }
            }
        }
    }
}
