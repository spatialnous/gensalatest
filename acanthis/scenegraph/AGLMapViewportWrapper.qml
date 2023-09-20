// Copyright (C) 2023 - 2023 Petros Koutsolampros

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
import QtQuick.Controls

import acanthis 1.0

Page {
    padding: 5
    property int viewID: -1
    property bool active: false
    property var graphViewModel
    background: Rectangle {
        //color: settings.glViewBackgroundColour
        border {
            color: active ? "red" : Theme.panelColour
            width: 5
        }
    }
    function update() {
        contentItem.children[0].update();
    }

    AGLMapViewport {
        anchors.fill: parent
        visible: true

        graphViewModel: parent.parent.graphViewModel
        foregroundColour: settings.glViewForegroundColour
        backgroundColour: settings.glViewBackgroundColour
        antialiasingSamples: settings.glViewAntialiasingSamples
        highlightOnHover: settings.glViewHighlightOnHover

        // it is necessary to "flip" the FBO here because the default assumes
        // that y is already flipped. Instead this will be handled internally
        // to be in line with depthmapX
        mirrorVertically: true

        focus: true

        onMousePressed: {
            graphViews.makeActive(parent.parent.viewID);
        }

        function split() {
            console.log("zomfg! " + parent.parent.parent);
        }
    }
}
