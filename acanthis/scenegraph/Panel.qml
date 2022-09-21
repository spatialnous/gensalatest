import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T

import "." as Ui

Page {
    id: root
    objectName: "panel"
    topPadding: 0
    bottomPadding: 0

    property bool expanded: true
    property T.Popup settingsPopup: null
    property alias settingsPopupToolButton: settingsPopupToolButton

    header: RowLayout {
        objectName: root.objectName + "Header"
        spacing: 0

        Label {
            objectName: parent.objectName + "TitleLabel"
            text: root.title
            font.bold: true
            color: Theme.panelTextColour

            Layout.leftMargin: 16
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            id: settingsPopupToolButton
            objectName: root.objectName + "SettingsToolButton"

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
            objectName: root.objectName + "HideShowToolButton"
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
}
