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

import acanthis 1.0

SplitView {
    SplitView.fillHeight: true
    SplitView.fillWidth: true
    handle: Rectangle {
        implicitWidth: 4
        implicitHeight: 4
        color: Theme.panelColour
    }
    property int minimumItemWidth: 20
    property int minimumItemHeight: 20
    property var viewModels

    function update() {
        for (var i = 0; i < this.count; ++i)
            this.itemAt(i).update()
    }

    function splitView(orientation, index) {
        let currentView = this.itemAt(index)
        if (currentView instanceof AGLSplitView) {
            currentView.splitView(orientation)
            return
        }

        if (this.orientation !== orientation) {
            return this.addAGLSplitView(index)
        } else if (this.orientation === Qt.Horizontal) {
            let graphViewModel = viewModels.createViewModel()
            return this.addAGLMapViewHorizontal(graphViewModel, index + 1)
        } else if (this.orientation === Qt.Vertical) {
            let graphViewModel = viewModels.createViewModel()
            return this.addAGLMapViewVertical(graphViewModel, index + 1)
        }
    }

    function addAGLSplitView(index) {
        let currentView = this.itemAt(index)

        let aglSplitViewComponent = Qt.createComponent("AGLSplitView.qml")
        let newSplitView = {
            "orientation": Qt.Horizontal,
            "SplitView.preferredWidth": currentView.width,
            "SplitView.preferredHeight": currentView.height,
            "viewModels": viewModels
        }
        if (this.orientation === Qt.Horizontal) {
            newSplitView.orientation = Qt.Vertical
            newSplitView["SplitView.preferredWidth"] = currentView.width
            newSplitView["SplitView.preferredHeight"] = currentView.height
        }

        let aglSplitView = aglSplitViewComponent.createObject(null,
                                                              newSplitView)
        if (aglSplitView === null) {
            console.log("Error creating AGLSplitView")
        }

        this.takeItem(index)
        this.insertItem(index, aglSplitView)

        aglSplitView.addItem(currentView)
        if (this.orientation === Qt.Vertical) {
            let graphViewModel = viewModels.createViewModel()
            return aglSplitView.addAGLMapViewHorizontal(graphViewModel, 1)
        } else {
            let graphViewModel = viewModels.createViewModel()
            return aglSplitView.addAGLMapViewVertical(graphViewModel, 1)
        }
    }

    function createMapView(width, height, graphViewModel) {
        let aglMapViewComponent = Qt.createComponent(
                "AGLMapViewportWrapper.qml")

        let aglViewComponentData = {
            "viewID": graphViewModel.id,
            "SplitView.minimumWidth": minimumItemWidth,
            "SplitView.minimumHeight": minimumItemHeight,
            "graphViewModel": graphViewModel
        }

        // preferred widths/height are used as those also change from the handle
        aglViewComponentData["SplitView.preferredWidth"] = width
        aglViewComponentData["SplitView.preferredHeight"] = height

        // an initial width/height is required for the first element to have it
        aglViewComponentData["width"] = width
        aglViewComponentData["height"] = height

        let newMapView = aglMapViewComponent.createObject(null,
                                                          aglViewComponentData)
        if (aglMapViewComponent.status === Component.Error)
            console.debug("Error: " + aglMapViewComponent.errorString())

        return newMapView
    }

    function getNewWidths() {
        if (minimumItemWidth * (this.count + 1) > parent.width) {
            console.error("Insufficient space to create new Map View")
            return []
        }

        let newItemWidth = parent.width / (this.count + 1)
        let prevItemsWidth = parent.width - newItemWidth
        let prevItemsNewWidth = new Array(this.count + 1)
        for (var i = 0; i < this.count; ++i) {
            let prevWidthPrc = this.itemAt(i).width / parent.width
            prevItemsNewWidth[i] = prevItemsWidth * prevWidthPrc
        }
        prevItemsNewWidth[this.count] = newItemWidth

        return prevItemsNewWidth
    }

    function addAGLMapViewHorizontal(graphViewModel, index) {
        let newWidths = getNewWidths()
        for (var j = 0; j < this.count; ++j) {
            this.itemAt(j).SplitView.preferredWidth = newWidths[j]
        }
        let newItemWidth = newWidths[newWidths.length - 1]

        let newMapView = createMapView(newItemWidth, parent.height,
                                       graphViewModel)
        this.insertItem(index, newMapView)
        return {
            "view": newMapView,
            "model": graphViewModel
        }
    }

    function getNewHeights() {
        if (minimumItemHeight * (this.count + 1) > parent.height) {
            console.error("Insufficient space to create new Map View")
            return []
        }

        let newItemHeight = parent.height / (this.count + 1)
        let prevItemsHeight = parent.height - newItemHeight
        let prevItemsNewHeight = new Array(this.count + 1)
        for (var i = 0; i < this.count; ++i) {
            let prevHeightPrc = this.itemAt(i).height / parent.height
            prevItemsNewHeight[i] = prevItemsHeight * prevHeightPrc
        }
        prevItemsNewHeight[this.count] = newItemHeight

        return prevItemsNewHeight
    }

    function addAGLMapViewVertical(graphViewModel, index) {
        let newHeights = getNewHeights()
        for (var j = 0; j < this.count; ++j) {
            this.itemAt(j).SplitView.preferredHeight = newHeights[j]
        }
        let newItemHeight = newHeights[newHeights.length - 1]

        let newMapView = createMapView(parent.width, newItemHeight,
                                       graphViewModel)
        this.insertItem(index, newMapView)
        return {
            "view": newMapView,
            "model": graphViewModel
        }
    }
}
