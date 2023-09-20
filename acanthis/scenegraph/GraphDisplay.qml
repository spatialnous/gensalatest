import QtQuick
import QtQuick.Controls

ListView {
    // Instead of displaying one item at a time we load every
    // file's View (GL, Layers etc.) and only display the one
    // that is the current index.
    id: graphListGLView
    model: graphDisplayModel
    interactive: false
    snapMode: ListView.SnapOneItem

    function getModelView() {
        return this;
    }

    delegate: SplitView {
        id: mainSplitView
        objectName: "mainSplitView"
        anchors.top: parent == null ? window.top : parent.top
        width: parent == null ? 0 : parent.width
        height: parent == null ? 0 : parent.parent.height
        handle: Item {
            implicitWidth: 4
        }
        visible: currentIndex === index

        function splitActiveView(orientation) {
            graphViews.splitActiveView(orientation);
        }

        SplitView {
            id: graphViews
            SplitView.fillWidth: true
            focus: true

            property int nAGLViews: 0;
            property int activeMapViewID: -1;
            property var mapViews: ({});

            function nextAGLViewID() {
                let newAGLViewID = nAGLViews;
                nAGLViews = nAGLViews + 1;
                return newAGLViewID;
            }

            function splitActiveView(orientation) {
                if (activeMapViewID < 0) {
                    console.log("No active view selected")
                    return;
                }

                let activeMapView = mapViews[activeMapViewID];
                let aglSplitView = activeMapView.parent.parent;
                let activeMapViewIdx = Array.prototype.indexOf.call(aglSplitView.contentChildren, activeMapView)
                let newAGLViewID = nextAGLViewID();
                let newAGLView = aglSplitView.splitView(orientation, newAGLViewID, activeMapViewIdx);
                mapViews[newAGLViewID] = newAGLView;
            }

            function makeActive(viewID) {
                if (viewID < 0) return;
                if (activeMapViewID != -1) {
                    mapViews[activeMapViewID].active = false;
                }
                mapViews[viewID].active = true;
                activeMapViewID = viewID;
            }

            function redraw() {
                for (let i = 0; i < nAGLViews; i++) {
                    mapViews[i].update();
                }
            }

            function addAGLSplitView(parent, orientation, preferredWidth,
                                     preferredHeight) {
                let aglSplitViewComponent = Qt.createComponent(
                        "AGLSplitView.qml")

                let aglSplitView = aglSplitViewComponent.createObject(
                        parent, {
                            "orientation": orientation,
                            "width": parent.width,
                            "height": parent.height
                        });

                if (aglSplitView === null) {
                    console.log("Error creating AGLSplitView")
                }
                return aglSplitView
            }
            Component.onCompleted: {
                let out = ""
                for (var p in model['viewModels']) {
                    out += p + ': ' + model['viewModels'][p] + '\n';
                  }

                let aglSplitView = addAGLSplitView(this, Qt.Horizontal);

                let newAGLViewID = nextAGLViewID();
                let graphViewModel = viewModels.createViewModel()
                let newAGLView = aglSplitView.addAGLMapViewHorizontal(newAGLViewID, graphViewModel, 0);
                mapViews[newAGLViewID] = newAGLView;
                newAGLViewID = nextAGLViewID();
                graphViewModel = viewModels.createViewModel()
                newAGLView = aglSplitView.addAGLMapViewHorizontal(newAGLViewID, graphViewModel, 0);
                mapViews[newAGLViewID] = newAGLView;
            }
        }

        Loader {
            objectName: "mapsLoader"
            active: true
            visible: active
            sourceComponent: MapPanel {
                graphModel: graphModelFile
                graphViewModels: viewModels.models
            }
        }
    }
}
