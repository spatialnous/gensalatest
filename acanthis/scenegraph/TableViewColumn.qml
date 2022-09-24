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

import QtQuick 2.15

QtObject {
    property Item __view: null
    property int __index: -1
    property string title
    property string role
    property int width: (__view && __view.columnCount === 1) ? __view.width : 160
    property bool visible: true
    property bool resizable: true
    property bool movable: true
    property int elideMode: Text.ElideRight
    property int horizontalAlignment: Text.AlignLeft

    //property Component delegate

    property int accessibleRole: Accessible.ColumnHeader

    function resizeToContents() {
        var minWidth = 0
        var listdata = __view.__listView.children[0]
        for (var i = 0; __index === -1 && i < __view.__columns.length; ++i) {
            if (__view.__columns[i] === this)
                __index = i
        }
        // ### HACK We don't have direct access to the instantiated item,
        // so we go spelunking. Each 'item' variable check is annotated
        // with the expected object it should point to in BasicTableView.
        for (var row = 0 ; row < listdata.children.length ; ++row) {
            var item = listdata.children[row] ? listdata.children[row].rowItem : undefined
            if (item) { // FocusScope { id: rowitem }
                item = item.children[1]
                if (item) { // Row { id: itemrow }
                    item = item.children[__index]
                    if (item) { // Repeater.delegate a.k.a. __view.__itemDelegateLoader
                        var indent = __view.__isTreeView && __index === 0 ? item.__itemIndentation : 0
                        item  = item.item
                        if (item && item.hasOwnProperty("implicitWidth")) {
                            minWidth = Math.max(minWidth, item.implicitWidth + indent)
                        }
                    }
                }
            }
        }
        if (minWidth)
            width = minWidth
    }
}
