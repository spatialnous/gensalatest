import QtQuick 2.2

ListModel {
    property var document;
    property int nAGLViews: 0
    property int activeMapViewID: -1
    property var nextAGLViewID: function() {
        let newAGLViewID = this.nAGLViews;
        this.nAGLViews = this.nAGLViews + 1;
        return newAGLViewID;
    }
    property var createViewModel: function() {
        let newAGLViewID = this.nextAGLViewID();
        let newGraphViewModel = new GraphViewModel(newAGLViewID);
        newGraphViewModel.graphModel = document;
        // it doesn't seem like we can put the new GraphViewModel
        // directly into the qml model (it gets converted to
        // something qt?) so we have to have this intermediate
        // json that will end up creating a ListElement with a
        // single 'graphViewModel' parameter in it
        this.append({
           'graphViewModel': newGraphViewModel
        });
        return newGraphViewModel;
    }
}
