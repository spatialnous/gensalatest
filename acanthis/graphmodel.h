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

#pragma once

#include "maplayer.h"

#include "salalib/mgraph.h"

#include <QObject>

// This is a representation of the MetaGraph (the file itself) meant
// to be displayed over multiple views (viewports, lists etc.)
class GraphViewModel;
class GraphModel : public QObject {
    Q_OBJECT

    std::string m_filename;

  public:
    GraphModel(std::string filename);

    MetaGraph &getMetaGraph() const { return *m_metaGraph; }
    bool hasMetaGraph() const { return m_metaGraph.get() != nullptr; }

    // These are stored as QSharedPointers so that they can be used by the map view tree
    // which stores everything in QSharedPointers
    QList<QSharedPointer<MapLayer>> &getMapLayers() { return m_mapLayers; }
    const QList<QSharedPointer<MapLayer>> &getMapLayers() const { return m_mapLayers; }

    MapLayer *layerAt(std::size_t index) { return m_mapLayers[index].get(); }

    std::size_t layerCount() { return m_mapLayers.size(); }
    std::string getFilenameStr() { return m_filename; }
    Q_INVOKABLE QString getFilename() { return QString::fromStdString(m_filename); }
//    Q_INVOKABLE GraphViewModel* createViewModel();

  private:
    std::unique_ptr<MetaGraph> m_metaGraph = nullptr;
    QList<QSharedPointer<MapLayer>> m_mapLayers;
};
