// Copyright (C) 2020 Petros Koutsolampros

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

#include "segmentpathsmainwindow.h"
#include "segmentpathsgraphdoc.h"

#include "depthmapX/mainwindow.h"

#include <QMenuBar>

MainWindowPluginRegister<SegmentPathsMainWindow> SegmentPathsMainWindow::reg("MyModuleMainWindow");

bool SegmentPathsMainWindow::createMenus(MainWindow *mainWindow) {

    QMenu *toolsMenu = getOrAddRootMenu(mainWindow, tr("&Tools"));
    QMenu *segmentMenu = getOrAddMenu(toolsMenu, tr("&Segment"));
    QMenu *shortestPathsMenu = getOrAddMenu(segmentMenu, tr("Shortest Paths"));

    QAction *angularPathAct = new QAction(tr("Angular shortest path"), mainWindow);
    angularPathAct->setStatusTip(tr("Create an angular shortest path"));
    connect(angularPathAct, SIGNAL(triggered()), this, SLOT(OnAngularShortestPath()));
    shortestPathsMenu->addAction(angularPathAct);

    QAction *metricPathAct = new QAction(tr("Metric shortest path"), mainWindow);
    metricPathAct->setStatusTip(tr("Create a metric shortest path"));
    connect(metricPathAct, SIGNAL(triggered()), this, SLOT(OnMetricShortestPath()));
    shortestPathsMenu->addAction(metricPathAct);

    QAction *topoPathAct = new QAction(tr("Topological shortest path"), mainWindow);
    topoPathAct->setStatusTip(tr("Create a topological shortest path"));
    connect(topoPathAct, SIGNAL(triggered()), this, SLOT(OnTopoShortestPath()));
    shortestPathsMenu->addAction(topoPathAct);

    return true;
}

void SegmentPathsMainWindow::OnAngularShortestPath() {}

void SegmentPathsMainWindow::OnMetricShortestPath() {}

void SegmentPathsMainWindow::OnTopoShortestPath() {}
