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
import Qt.labs.settings 1.0
import QtQuick.Dialogs

import acanthis 1.0

import "." as Ui

ApplicationWindow {
    id: window
    flags: Qt.FramelessWindowHint
    property string appName: "acanthis"

    width: 800
    height: 600
    minimumHeight: 100
    minimumWidth: 200

    property int bw: 5

    visible: true

    function toggleMaximized() {
        if (window.visibility === Window.Maximized) {
            window.showNormal()
        } else {
            window.showMaximized()
        }
    }

    // The mouse area is just for setting the right cursor shape
    MouseArea {
        id: resizableMouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: {
            const p = Qt.point(mouseX, mouseY)
            const b = bw + 10
            // Increase the corner size slightly
            if (p.x < b && p.y < b)
                return Qt.SizeFDiagCursor
            if (p.x >= width - b && p.y >= height - b)
                return Qt.SizeFDiagCursor
            if (p.x >= width - b && p.y < b)
                return Qt.SizeBDiagCursor
            if (p.x < b && p.y >= height - b)
                return Qt.SizeBDiagCursor
            if (p.x < b || p.x >= width - b)
                return Qt.SizeHorCursor
            if (p.y < b || p.y >= height - b)
                return Qt.SizeVerCursor
        }
        acceptedButtons: Qt.NoButton // don't handle actual events
    }

    DragHandler {
        id: resizeHandler
        grabPermissions: TapHandler.TakeOverForbidden
        target: null
        onActiveChanged: if (active) {
                             const p = resizeHandler.centroid.position
                             const b = bw + 10
                             // Increase the corner size slightly
                             let e = 0
                             if (p.x < b) {
                                 e |= Qt.LeftEdge
                             }
                             if (p.x >= width - b) {
                                 e |= Qt.RightEdge
                             }
                             if (p.y < b) {
                                 e |= Qt.TopEdge
                             }
                             if (p.y >= resizableMouseArea.height - b) {
                                 e |= Qt.BottomEdge
                             }
                             if (e != 0) {
                                 window.startSystemResize(e)
                             }
                         }
    }

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
            DocumentManager.openDocument(openDialog.selectedFile)
            let lastDocumentIndex = DocumentManager.lastDocumentIndex()
            if (lastDocumentIndex === (DocumentManager.numOpenedDocuments(
                                           ) - 1)) {
                graphDisplayModel.append({
                                             "graphDocumentFile": DocumentManager.lastDocument,
                                             "current": true
                                         })
            }
            graphListNameView.currentIndex = lastDocumentIndex
        }
    }

    background: Rectangle {
        color: Theme.panelColour
    }
    Page {
        anchors.fill: parent
        anchors.margins: window.visibility === Window.Windowed ? bw : 0
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
                            width: (graphListNameView.width
                                    - (graphListNameView.spacing * (graphDisplayModel.count + 1)))
                                   / graphDisplayModel.count
                            height: parent == null ? 0 : parent.height
                            padding: 0
                            onClicked: {
                                graphListNameView.currentIndex = index
                            }
                            ToolTip.visible: hovered
                            ToolTip.delay: Theme.tooltipDelay
                            ToolTip.text: graphDocumentFile.getFilename()
                            property int listIndex: index

                            background: Rectangle {
                                color: {
                                    if (hovered) {
                                        graphListNameView.currentIndex === index ? Theme.activeTabHoverColour : Theme.inactiveTabHoverColour
                                    } else {
                                        graphListNameView.currentIndex === index ? Theme.activeTabColour : Theme.inactiveTabColour
                                    }
                                }
                                radius: {
                                    hovered || graphListNameView.currentIndex
                                            === index ? Theme.tabButtonHoverRadius : 0
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
                                        radius: parent.width * 0.5
                                        color: parent.hovered ? Theme.tabCloseButtonHoverColour : Theme.tabCloseButtonColour
                                    }
                                    onClicked: {
                                        graphDisplayModel.remove(index)
                                        DocumentManager.removeDocument(
                                                    listIndex)
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

                        TapHandler {
                            onTapped: if (tapCount === 2)
                                          toggleMaximized()
                            gesturePolicy: TapHandler.DragThreshold
                        }
                        DragHandler {
                            grabPermissions: TapHandler.CanTakeOverFromAnything
                            target: null
                            onActiveChanged: if (active) {
                                                 window.startSystemMove()
                                             }
                        }
                    }

                    Button {
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
                            radius: parent.width * 0.5
                            color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
                        }
                        onClicked: {
                            Qt.quit()
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
                            DocumentManager.createEmptyDocument()
                            graphDisplayModel.append({
                                                         "graphDocumentFile": DocumentManager.lastDocument,
                                                         "current": true
                                                     })
                            graphListNameView.currentIndex = DocumentManager.lastDocumentIndex()
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
                                            graphListNameView.currentIndex).graphDocumentFile.getFilename(
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
                height: parent == null ? 0 : parent.parent.height
                handle: Item {
                    implicitWidth: 4
                }
                visible: graphListNameView.currentIndex === index

                SplitView {
                    id: graphView
                    SplitView.fillWidth: true
                    focus: true

                    function addAGLSplitView(parent, orientation) {
                        let aglSplitViewComponent = Qt.createComponent(
                                "AGLSplitView.qml")
                        let aglSplitView = aglSplitViewComponent.createObject(
                                parent, {
                                    "orientation": orientation
                                })

                        if (aglSplitView === null) {
                            console.log("Error creating AGLSplitView")
                        }
                        return aglSplitView
                    }
                    Component.onCompleted: {
                        addAGLSplitView(this, Qt.Vertical)
                    }
                }

                Loader {
                    objectName: "mapsLoader"
                    active: true
                    visible: active
                    sourceComponent: MapPanel {
                        graphDocument: graphDocumentFile
                    }
                }
            }
        }
    }
}
