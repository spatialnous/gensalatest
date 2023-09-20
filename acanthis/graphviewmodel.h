// Copyright (C) 2023, Petros Koutsolampros

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

#include "graphmodel.h"

#include <QObject>
#include <qqml.h>

// This class is meant to hold display information about one view[port]
// of the graph document, such as which items are displayed

class GraphViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GraphModel *graphModel MEMBER m_graphModel NOTIFY graphModelChanged)

    GraphModel *m_graphModel;

public:
    Q_INVOKABLE explicit GraphViewModel(QObject *parent = nullptr) {};
    QList<QSharedPointer<MapLayer>> &getMapLayers() { return m_graphModel->getMapLayers(); }
    const QList<QSharedPointer<MapLayer>> &getMapLayers() const { return m_graphModel->getMapLayers(); }
    bool hasMetaGraph() const { return m_graphModel->hasMetaGraph(); }
    const QtRegion getBoundingBox() const { return m_graphModel->getMetaGraph().getBoundingBox(); }

signals:
    void graphModelChanged();
};
