// Copyright (C) 2020, Petros Koutsolampros

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

// Original found here: https://stackoverflow.com/a/582456

#pragma once

#include "depthmapX/mainwindowplugin.h"

#include <iostream>

template <typename T> MainWindowPlugin *createT() { return new T; }

struct MainWindowPluginFactory {
    typedef std::map<std::string, MainWindowPlugin *(*)()> PluginMap;

    static MainWindowPlugin *createInstance(std::string const &s) {
        PluginMap::iterator it = getMap()->find(s);
        if (it == getMap()->end())
            return 0;
        return it->second();
    }

  public:
    static PluginMap *getMap() {
        // never deleted. (exist until program termination)
        // because we can't guarantee correct destruction order
        if (!map) {
            map = new PluginMap;
        }
        return map;
    }

  private:
    static PluginMap *map;
};

template <typename T> struct MainWindowPluginRegister : MainWindowPluginFactory {
    MainWindowPluginRegister(std::string const &s) { getMap()->insert(std::make_pair(s, &createT<T>)); }
};
