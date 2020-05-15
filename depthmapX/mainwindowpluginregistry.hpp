// Copyright (C) 2017, Christian Sailer
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

#pragma once

#include "imainwindowmodulefactory.h"
#include "imainwindowplugin.h"
#include <memory>
#include <vector>

class MainWindowPluginRegistry : public IMainWindowPluginFactory {
  public:
    MainWindowPluginRegistry() { populatePlugins(); }

    const MainWindowPluginVec &getPlugins() const { return m_availablePlugins; }

  private:
    void populatePlugins();
    MainWindowPluginVec m_availablePlugins;
};

#define REGISTER_MAIN_WINDOW_MODULE(module)                                                                           \
    m_availablePlugins.push_back(std::unique_ptr<IMainWindowPlugin>(new module));
