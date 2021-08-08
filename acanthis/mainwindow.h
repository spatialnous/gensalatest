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

#include <QMainWindow>
#include <QMdiArea>

class MainWindow : public QMainWindow {
    Q_OBJECT

    struct GraphWindows {
        GraphDocument m_graphDocument;
        std::vector<QMdiSubWindow *> m_windows;
        QMdiSubWindow *m_activeWindow;
        GraphWindows(std::string fileName) : m_graphDocument(fileName){};
    };

  public:
    explicit MainWindow(const std::string &fileToLoad, Settings &settings, QWidget *parent = 0);
    void loadFile(const std::string fileName);

  private:
    QMdiArea *m_mdiArea;
    std::map<std::string, GraphWindows> m_graphsAndWindows;
};
