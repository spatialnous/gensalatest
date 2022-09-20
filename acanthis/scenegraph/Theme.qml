pragma Singleton

import QtQuick 2.12

QtObject {
    property color focusColour: "red"
    property color panelColour: "#555555"
    property color toolbarButtonColour: "transparent"
    property color toolbarButtonHoverColour: "#777777"
    property color toolbarButtonTextColour: "white"
    property color activeTabColour: "#888888"
    property color activeTabHoverColour: activeTabColour
    property color inactiveTabColour: "transparent"
    property color inactiveTabHoverColour: toolbarButtonHoverColour
    property color tabCloseButtonColour: "transparent"
    property color tabCloseButtonHoverColour: "#828282"
    property color appNameColour: "#dddddd"
    property int tooltipDelay: Qt.styleHints.mousePressAndHoldInterval
    property int tabButtonHoverRadius: 5
}
