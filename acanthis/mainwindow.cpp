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

#include "interfaceversion.h"

#include <QAction>
#include <QDockWidget>
#include <QListWidget>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(const std::string &fileToLoad, Settings &settings) : QQuickView() {

    //    m_mdiArea = new QMdiArea(this);

    //    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //    setCentralWidget(m_mdiArea);

    //    setWindowTitle(TITLE_BASE);

    createActions();
    createDockWindows();

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

void MainWindow::createActions() {
    //    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    //    QToolBar *fileToolBar = addToolBar(tr("File"));

    //    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    //    QAction *newFileAct = new QAction(newIcon, tr("&New Letter"), this);
    //    newFileAct->setShortcuts(QKeySequence::New);
    //    newFileAct->setStatusTip(tr("Create a new graph file"));
    //    connect(newFileAct, &QAction::triggered, this, &MainWindow::newFile);
    //    fileMenu->addAction(newFileAct);
    //    fileToolBar->addAction(newFileAct);

    //    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    //    QAction *saveFileAct = new QAction(saveIcon, tr("&Save..."), this);
    //    saveFileAct->setShortcuts(QKeySequence::Save);
    //    saveFileAct->setStatusTip(tr("Save the current graph file"));
    //    connect(saveFileAct, &QAction::triggered, this, &MainWindow::saveFile);
    //    fileMenu->addAction(saveFileAct);
    //    fileToolBar->addAction(saveFileAct);

    //    fileMenu->addSeparator();

    //    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    //    quitAct->setShortcuts(QKeySequence::Quit);
    //    quitAct->setStatusTip(tr("Quit the application"));

    //    m_menus.emplace("View", menuBar()->addMenu(tr("&View")));

    //    menuBar()->addSeparator();

    //    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    //    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::toggleAboutDialog);
    //    aboutAct->setStatusTip(tr("Show the application's About dialog"));
}

void MainWindow::createDockWindows() {

    //    QDockWidget *dock = new QDockWidget(tr("Maps"), this);
    //    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //    QListWidget *customerList = new QListWidget(dock);
    //    customerList->addItems(QStringList() << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
    //                                         << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
    //                                         << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
    //                                         << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
    //                                         << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
    //                                         << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
    //    dock->setWidget(customerList);
    //    addDockWidget(Qt::LeftDockWidgetArea, dock);
    //    m_menus["View"]->addAction(dock->toggleViewAction());

    //    dock = new QDockWidget(tr("Attributes"), this);
    //    QListWidget *paragraphsList = new QListWidget(dock);
    //    paragraphsList->addItems(QStringList() << "Thank you for your payment which we have received today."
    //                                           << "Your order has been dispatched and should be with you "
    //                                              "within 28 days."
    //                                           << "We have dispatched those items that were in stock. The "
    //                                              "rest of your order will be dispatched once all the "
    //                                              "remaining items have arrived at our warehouse. No "
    //                                              "additional shipping charges will be made."
    //                                           << "You made a small overpayment (less than $5) which we "
    //                                              "will keep on account for you, or return at your request."
    //                                           << "You made a small underpayment (less than $1), but we have "
    //                                              "sent your order anyway. We'll add this underpayment to "
    //                                              "your next bill."
    //                                           << "Unfortunately you did not send enough money. Please remit "
    //                                              "an additional $. Your order will be dispatched as soon as "
    //                                              "the complete amount has been received."
    //                                           << "You made an overpayment (more than $5). Do you wish to "
    //                                              "buy more items, or should we return the excess to you?");
    //    dock->setWidget(paragraphsList);
    //    addDockWidget(Qt::LeftDockWidgetArea, dock);
    //    m_menus["View"]->addAction(dock->toggleViewAction());

    //    //    connect(customerList, &QListWidget::currentTextChanged, this, &MainWindow::insertCustomer);
    //    //    connect(paragraphsList, &QListWidget::currentTextChanged, this, &MainWindow::addParagraph);
}
