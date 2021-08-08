// Copyright (C) 2021 Petros Koutsolampros

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

#include "mainwindow.h"

MainWindow::MainWindow(const std::string &fileToLoad, Settings &settings, QWidget *parent) : QMainWindow(parent) {

    m_mdiArea = new QMdiArea(this);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(m_mdiArea);

    if (!fileToLoad.empty()) {
        loadFile(fileToLoad);
    }
}

void MainWindow::loadFile(const std::string fileName) {
    auto existingGraphIter = m_graphsAndWindows.find(fileName);
    if (existingGraphIter != m_graphsAndWindows.end()) {
        m_mdiArea->setActiveSubWindow(existingGraphIter->second.m_activeWindow);
        return;
    }
    GraphWindows &newGraphWindows = m_graphsAndWindows.emplace(fileName, GraphWindows{fileName}).first->second;
}
