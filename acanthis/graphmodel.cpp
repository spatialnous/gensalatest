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

#include "graphmodel.h"

#include "graphviewmodel.h"
#include "pixelmaplayer.h"
#include "shapegraphlayer.h"
#include "shapemaplayer.h"

#include <QVariant>

GraphModel::GraphModel(std::string filename) : m_filename(filename) {
    m_metaGraph = std::unique_ptr<MetaGraph>(new MetaGraph(filename));
    m_metaGraph->readFromFile(filename);
    for (ShapeMap &shapeMap : m_metaGraph->getDataMaps()) {
        m_mapLayers.append(QSharedPointer<ShapeMapLayer>(new ShapeMapLayer(shapeMap)));
    }
    for (auto &drawingFile : m_metaGraph->m_drawingFiles) {
        for (ShapeMap &shapeMap : drawingFile.m_spacePixels) {
            m_mapLayers.append(QSharedPointer<ShapeMapLayer>(new ShapeMapLayer(shapeMap)));
        }
    }
    for (auto &shapeGraph : m_metaGraph->getShapeGraphs()) {
        m_mapLayers.append(QSharedPointer<ShapeGraphLayer>(new ShapeGraphLayer(*shapeGraph)));
    }
    for (PointMap &pointMap : m_metaGraph->getPointMaps()) {
        m_mapLayers.append(QSharedPointer<PixelMapLayer>(new PixelMapLayer(pointMap)));
    }
}

//GraphViewModel* GraphModel::createViewModel() {
//    auto gvm = new GraphViewModel();
//    gvm->setProperty("graphModel", QVariant::fromValue(this));
//    return gvm;
//};