import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ListView {
    // Instead of displaying one item at a time we load every
    // file's View (GL, Layers etc.) and only display the one
    // that is the current index.
    interactive: false
    snapMode: ListView.SnapOneItem

    function getModelView() {
        return this
    }

    onCountChanged: function (index) {
        for (var i = 0; i < model.count; i++) {
            if (i !== currentIndex)
                itemAtIndex(i).visible = false
            else
                itemAtIndex(i).visible = true
        }
    }

    onCurrentIndexChanged: {
        // ideally this should not be necessary, but doing this
        // in the delegate (visible: currentIndex === index)
        // does not work
        for (var i = 0; i < model.count; i++) {
            if (i !== currentIndex)
                itemAtIndex(i).visible = false
            else
                itemAtIndex(i).visible = true
        }
    }

    delegate: SplitView {
        id: mainSplitView
        objectName: "mainSplitView"
        anchors.top: parent == null ? window.top : parent.top
        width: parent == null ? 0 : parent.width
        height: parent == null ? 0 : parent.parent.height
        handle: PanelHandle {
            orientation: Qt.Vertical
        }

        function splitActiveView(orientation) {
            graphViews.splitActiveView(orientation)
        }

        SplitView {
            id: graphViews
            SplitView.fillWidth: true
            focus: true

            property var mapViews: ({})

            function splitActiveView(orientation) {
                if (views.activeMapViewID < 0) {
                    console.log("No active view selected")
                    return
                }

                let activeMapView = mapViews[views.activeMapViewID]
                let aglSplitView = activeMapView.parent.parent
                let activeMapViewIdx = Array.prototype.indexOf.call(
                        aglSplitView.contentChildren, activeMapView)
                let newAGLView = aglSplitView.splitView(orientation,
                                                        activeMapViewIdx)

                mapViews[newAGLView.model.id] = newAGLView.view
            }

            function makeActive(viewID) {
                if (viewID < 0)
                    return
                if (views.activeMapViewID !== -1) {
                    mapViews[views.activeMapViewID].active = false
                }
                mapViews[viewID].active = true
                views.activeMapViewID = viewID

                // iterate to find the index of the view with this viewID
                let idx = 0
                for (var i = 0; i < views.count; i++) {
                    if (views.get(i).graphViewModel.id === viewID) {
                        idx = i
                        break
                    }
                }

                mapsPanel.item.contentItem.setCurrentIndex(idx)
            }

            function redraw() {
                for (var i = 0; i < views.nAGLViews; i++) {
                    mapViews[i].update()
                }
            }

            function addAGLSplitView(parent, orientation, preferredWidth, ///
                                     preferredHeight) {
                let aglSplitViewComponent = Qt.createComponent(
                        "AGLSplitView.qml")

                let aglSplitViewProperties = {
                    "viewModels": model.views,
                    "orientation": orientation,
                    "width": parent.width,
                    "height": parent.height
                }
                let aglSplitView = aglSplitViewComponent.createObject(
                        parent, aglSplitViewProperties)

                if (aglSplitView === null) {
                    console.log("Error creating AGLSplitView")
                }
                return aglSplitView
            }
            Component.onCompleted: {
                let aglSplitView = addAGLSplitView(this, Qt.Horizontal)
                let graphViewModel = views.createViewModel()
                let newAGLView = aglSplitView.addAGLMapViewHorizontal(
                        graphViewModel, 0)
                mapViews[graphViewModel.id] = newAGLView.view
                graphViewModel = views.createViewModel()
                newAGLView = aglSplitView.addAGLMapViewHorizontal(
                            graphViewModel, 0)
                mapViews[graphViewModel.id] = newAGLView.view
            }
        }

        Loader {
            objectName: "mapsLoader"
            id: mapsPanel
            active: true
            visible: active
            sourceComponent: MapPanel {
                graphModel: graphModelFile
                viewsModel: views
            }
        }
    }
}
