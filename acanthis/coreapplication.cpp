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

#include "coreapplication.h"

#include "agl/view/aglmapviewport.h"
#include "aqmapviewmodel.h"
#include "settingsimpl.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuick/QQuickView>

int CoreApplication::exec() {
    SettingsImpl settings(new DefaultSettingsFactory);

    if (!settings.readSetting(SettingTag::licenseAccepted, false).toBool()) {
        //        auto dummy = MainWindowFactory::getLicenseDialog();
        //        dummy->setModal(true);
        //        dummy->setWindowTitle(TITLE_BASE);
        //        dummy->exec();
        //        if (dummy->result() == QDialog::Rejected) {
        //            return 0;
        //        }
        settings.writeSetting(SettingTag::licenseAccepted, true);
    }

    auto args = arguments();
    std::string fileToLoad = m_fileToLoad;
    if (args.length() == 2) {
        fileToLoad = args[1].toStdString();
    }

    setOrganizationName("acanthis");
    setOrganizationDomain("acanth.is");
    setApplicationName("acanthis");

    int versionMajor = 1;
    int versionMinor = 0;

    qmlRegisterType<AGLMapViewport>("acanthis", versionMajor, versionMinor, "AGLMapViewport");
//    qmlRegisterType<GraphViewModel>("acanthis", versionMajor, versionMinor, "GraphViewModel");
    qmlRegisterType<AQMapViewModel>("acanthis", versionMajor, versionMinor, "AQMapViewModel");
    qmlRegisterSingletonType<DocumentManager>(
                "acanthis", versionMajor, versionMinor, "DocumentManager",
                [&](QQmlEngine *, QJSEngine *) -> QObject * {
        return new DocumentManager;
        // the QML engine takes ownership of the singleton
    });

    qmlRegisterUncreatableType<GraphModel>(
        "acanthis", versionMajor, versionMinor, "GraphModel",
        QLatin1String("Cannot create objects of type GraphModel"));

    qmlRegisterSingletonType(QUrl("qrc:///scenegraph/Theme.qml"), "acanthis",
                             versionMajor, versionMinor, "Theme");

    QQmlApplicationEngine engine;

    QJSValue jsMetaObject = engine.newQMetaObject(&GraphViewModel::staticMetaObject);
    engine.globalObject().setProperty(GraphViewModel::staticMetaObject.className(), jsMetaObject);

    engine.load(QUrl(QStringLiteral("qrc:/scenegraph/MainWindow.qml")));
    //    mMainWindow->setResizeMode(QQuickView::SizeRootObjectToView);
    //    engine->show();

    //    mMainWindow = MainWindowFactory::getMainWindow(fileToLoad, settings);
    //    mMainWindow->setResizeMode(QQuickView::SizeRootObjectToView);
    //    mMainWindow->setSource(QUrl("qrc:///scenegraph/main.qml"));
    //    mMainWindow->show();


    QGuiApplication::setWindowIcon(QIcon(":/images/acanthis.svg"));

    return QGuiApplication::exec();
}
