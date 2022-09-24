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

pragma Singleton

import QtQuick 2.12

QtObject {
    property color focusColour: "red"
    property color panelColour: "#555555"
    property color panelTextColour: "#FFFFFF"
    property color toolbarButtonColour: "transparent"
    property color toolbarButtonHoverColour: "#777777"
    property color toolbarButtonTextColour: "white"
    property color toolbarButtonTextDisabledColour: "#777777"
    property color activeTabColour: "#888888"
    property color activeTabHoverColour: activeTabColour
    property color inactiveTabColour: "#444444"
    property color inactiveTabHoverColour: toolbarButtonHoverColour
    property color tabCloseButtonColour: "transparent"
    property color tabCloseButtonHoverColour: "#828282"
    property color appNameColour: "#dddddd"
    property int tooltipDelay: Qt.styleHints.mousePressAndHoldInterval
    property int tabButtonHoverRadius: 5
}
