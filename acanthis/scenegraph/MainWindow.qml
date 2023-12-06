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

    // list of graph documents
    ListModel {
        id: graphDisplayModel
    }

    property list<QtObject> graphDisplayModelViews

    function registerDisplayModelView(displayModelView) {
        graphDisplayModelViews.push(displayModelView)
    }

    function appendDocumentToDisplayModel(document) {
        var newListModel = Qt.createComponent("ViewListModel.qml").createObject(
                    graphDisplayModel)
        newListModel.document = document

        graphDisplayModel.append({
                                     "graphModelFile": document,
                                     "current": true,
                                     "views": newListModel
                                 })
    }

    function openDocument(document) {
        DocumentManager.openDocument(document)
        let lastDocumentIndex = DocumentManager.lastDocumentIndex()
        if (lastDocumentIndex === (DocumentManager.numOpenedDocuments() - 1)) {
            appendDocumentToDisplayModel(DocumentManager.lastDocument)
        }
        for (var i = 0; i < graphDisplayModelViews.length; i++) {
            console.log(i, graphDisplayModelViews[i])
            graphDisplayModelViews[i].currentIndex = lastDocumentIndex
        }
    }

    function newDocument() {
        DocumentManager.createEmptyDocument()
        appendDocumentToDisplayModel(DocumentManager.lastDocument)
        for (var i = 0; i < graphDisplayModelViews.length; i++) {
            graphDisplayModelViews[i].currentIndex = DocumentManager.lastDocumentIndex()
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

                TopBar {
                    Layout.leftMargin: toolbar.spacing
                    Layout.topMargin: toolbar.spacing
                    Layout.rightMargin: toolbar.spacing
                    Layout.bottomMargin: 0
                    Component.onCompleted: {
                        registerDisplayModelView(this.getModelView())
                    }
                }
                MainToolBar {
                    Layout.leftMargin: toolbar.spacing
                    Layout.topMargin: 0
                    Layout.rightMargin: toolbar.spacing
                    Layout.bottomMargin: toolbar.spacing
                    //                    Component.onCompleted: {
                    //                        registerDisplayModelView(this)
                    //                    }
                }
            }
        }

        GraphDisplay {
            id: graphListGLView
            model: graphDisplayModel
            anchors.fill: parent
            Component.onCompleted: {
                registerDisplayModelView(this.getModelView())
            }
        }
    }
}
