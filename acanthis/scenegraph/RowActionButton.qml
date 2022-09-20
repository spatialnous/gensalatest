import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import "." as Ui

Button {
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
}
