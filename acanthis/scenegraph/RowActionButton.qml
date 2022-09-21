import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import "." as Ui

Button {
    id: rowButton
    text: "+"
    flat: true
    focusPolicy: Qt.NoFocus
    hoverEnabled: true

    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.maximumWidth: implicitHeight

    ToolTip.visible: hovered
    //ToolTip.delay: UiConstants.toolTipDelay
    //ToolTip.timeout: UiConstants.toolTipTimeout

    contentItem: Text {
        text: rowButton.text
        horizontalAlignment: Text.AlignHCenter
        color: rowButton.enabled ? Theme.toolbarButtonTextColour : Theme.toolbarButtonTextDisabledColour
    }

    background: Rectangle {
        Layout.fillHeight: true
        implicitWidth: parent.height
        radius: Theme.tabButtonHoverRadius
        color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
    }
}
