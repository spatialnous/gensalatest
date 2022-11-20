// Copyright (C) 2017, Petros Koutsolampros

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

#include "aglshapegraph.h"

void AGLShapeGraph::loadGLObjects() {
    AGLShapeMap::loadGLObjects();
    m_glGraph.setNodeSize(m_shapeGraph.getSpacing() * 0.05);
    m_glGraph.setGraphCornerRadius(m_shapeGraph.getSpacing() * 0.3);

    std::vector<Connector> &connections = m_shapeGraph.getConnections();
    auto &shapes = m_shapeGraph.getAllShapes();
    auto shapeIter = shapes.begin();
    for (Connector &connector : connections) {
        auto &fromShape = shapeIter->second;
        auto fromCentroid = fromShape.getCentroid();
        for (int &connection : connector.m_connections) {
            auto &toShape = shapes[connection];
            auto toCentroid = toShape.getCentroid();
            m_glGraph.addConnection(
                SimpleLine(fromCentroid.x, fromCentroid.y, toCentroid.x, toCentroid.y),
                intersection_point(fromShape.getLine(), toShape.getLine()));
        }
        shapeIter++;
    }
    m_glGraph.setLinks(m_shapeGraph.getAllLinkLines());
    m_glGraph.setUnlinks(m_shapeGraph.getAllUnlinkPoints());
    m_glGraph.loadGLObjects();
}
