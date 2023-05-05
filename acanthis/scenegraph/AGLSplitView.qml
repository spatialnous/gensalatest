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
import QtQuick.Controls

import acanthis 1.0

SplitView {
    SplitView.fillHeight: true
    SplitView.fillWidth: true
    handle: Item {
        implicitWidth: 4
    }

    function update() {
        first.update();
        last.update();
    }

    AGLMapView {
        id: first
        SplitView.preferredWidth: parent.width / 2
        SplitView.preferredHeight: parent.height / 2
        graphDocument: graphDocumentFile
        foregroundColour: settings.glViewForegroundColour
        backgroundColour: settings.glViewBackgroundColour
        antialiasingSamples: 0
        highlightOnHover: true

        // it is necessary to "flip" the FBO here because the default assumes
        // that y is already flipped. Instead this will be handled internally
        // to be in line with depthmapX
        mirrorVertically: true

        focus: true
    }

    AGLMapView {
        id: last
        SplitView.preferredWidth: parent.width / 2
        SplitView.preferredHeight: parent.height / 2
        graphDocument: graphDocumentFile
        foregroundColour: settings.glViewForegroundColour
        backgroundColour: settings.glViewBackgroundColour
        antialiasingSamples: 0
        highlightOnHover: true

        // it is necessary to "flip" the FBO here because the default assumes
        // that y is already flipped. Instead this will be handled internally
        // to be in line with depthmapX
        mirrorVertically: true
    }
}
