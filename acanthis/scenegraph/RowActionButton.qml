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
import QtQuick.Layouts
import QtQuick.Controls

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
