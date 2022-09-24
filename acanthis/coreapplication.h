// Copyright (C) 2017 Petros Koutsolampros

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

#include "documentmanager.h"
#include "interfaceversion.h"
#include "mainwindow.h"
#include "settingsimpl.h"

#include <QFileOpenEvent>
#include <QGuiApplication>
#include <QtDebug>

class CoreApplication : public QGuiApplication {
  private:
    std::string m_fileToLoad;
    std::unique_ptr<MainWindow> m_mainWindow;
    DocumentManager m_documentManager;

  public:
    CoreApplication(int &argc, char **argv) : QGuiApplication(argc, argv) {}

    bool event(QEvent *event) {
        // this event is triggered in macOS, either by calling "Open with..."
        // in Finder, or by dropping a file on the acanthis icon on the dock
        // more info: http://doc.qt.io/qt-5/qfileopenevent.html
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            m_fileToLoad = openEvent->file().toStdString();
            m_mainWindow->loadFile(m_fileToLoad);
        }

        return QGuiApplication::event(event);
    }

    int exec();
};
