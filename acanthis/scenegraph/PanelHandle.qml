import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property int orientation: Qt.Vertical

    implicitWidth: orientation === Qt.Vertical ? 5 : parent.width
    implicitHeight: orientation === Qt.Vertical ? parent.height : 5

    Rectangle {
        anchors.fill: parent
        color: Theme.panelColour
    }

    GridLayout {
        anchors.fill: parent

        rowSpacing: 0
        columnSpacing: 0

        rows: orientation === Qt.Vertical ? 5 : 1
        columns: orientation === Qt.Vertical ? 1 : 5

        property int longDimension: {
            orientation === Qt.Vertical ? parent.height : parent.width
        }
        property real handleLength: longDimension / 7
        property real handleExtensionLength: longDimension / 7
        property real remainderLength: {
            (longDimension ///
             - (2 * handleExtensionLength) ///
             - handleLength) / 2
        }
        Rectangle {
            implicitWidth: orientation === Qt.Vertical ? 0 : parent.remainderLength
            implicitHeight: orientation === Qt.Vertical ? parent.remainderLength : 0
        }
        Rectangle {
            color: Theme.inactiveTabColour
            implicitWidth: orientation === Qt.Vertical ? 1 : parent.handleExtensionLength
            implicitHeight: orientation === Qt.Vertical ? parent.handleExtensionLength : 1
            Layout.alignment: Qt.AlignCenter
        }
        Rectangle {
            color: Theme.inactiveTabColour
            implicitWidth: orientation === Qt.Vertical ? 5 : parent.handleLength
            implicitHeight: orientation === Qt.Vertical ? parent.handleLength : 5
            Layout.alignment: Qt.AlignCenter
        }
        Rectangle {
            color: Theme.inactiveTabColour
            implicitWidth: orientation === Qt.Vertical ? 1 : parent.handleExtensionLength
            implicitHeight: orientation === Qt.Vertical ? parent.handleExtensionLength : 1
            Layout.alignment: Qt.AlignCenter
        }
        Rectangle {
            implicitWidth: orientation === Qt.Vertical ? 0 : parent.remainderLength
            implicitHeight: orientation === Qt.Vertical ? parent.remainderLength : 0
        }
    }
}
