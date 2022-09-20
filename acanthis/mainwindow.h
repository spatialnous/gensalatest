// Copyright (C) 2021 Petros Koutsolampro

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

#include "settings.h"

#include "graphdocument.h"

//#include <QMdiArea>
#include <QtQuick/QQuickView>

class MainWindow : public QQuickView {
    Q_OBJECT

    //    struct GraphWindows {
    //        GraphDocument m_graphDocument;
    //        std::vector<QMdiSubWindow *> m_windows;
    //        QMdiSubWindow *m_activeWindow;
    //        GraphWindows(std::string fileName) : m_graphDocument(fileName){};
    //    };

    //    std::map<std::string, GraphWindows> m_graphsAndWindows;
    GraphDocument m_graphDocument;

  public:
    explicit MainWindow(const std::string &fileToLoad, Settings &settings);
    void loadFile(const std::string fileName);
    void newFile(){};
    void saveFile(){};
    Settings &getSettings() { return m_settings; }
    GraphDocument &getGraphDocument() { return m_graphDocument; }

  private:
    //    QMdiArea *m_mdiArea;
    Settings &m_settings;
    //    std::map<std::string, QMenu *> m_menus;

  private: // functions
    void createActions();
    void createDockWindows();
    void toggleAboutDialog(){};
};
