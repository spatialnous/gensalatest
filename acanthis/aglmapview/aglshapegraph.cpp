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

#include "salalib/geometrygenerators.h"

void AGLShapeGraph::loadGLObjects() {
    AGLShapeMap::loadGLObjects();
    const std::vector<SimpleLine> &linkLines = m_shapeGraph.getAllLinkLines();
    std::vector<Point2f> linkPointLocations;
    for (auto &linkLine : linkLines) {
        linkPointLocations.push_back(linkLine.start());
        linkPointLocations.push_back(linkLine.end());
    }

    const std::vector<Point2f> &linkFillTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(32, m_shapeGraph.getSpacing() * 0.1,
                                                          linkPointLocations);
    m_linkFills.loadTriangleData(linkFillTriangles, qRgb(0, 0, 0));

    std::vector<SimpleLine> linkFillPerimeters = GeometryGenerators::generateMultipleCircleLines(
        32, m_shapeGraph.getSpacing() * 0.1, linkPointLocations);
    linkFillPerimeters.insert(linkFillPerimeters.end(), linkLines.begin(), linkLines.end());
    m_linkLines.loadLineData(linkFillPerimeters, qRgb(0, 255, 0));

    const std::vector<Point2f> &unlinkPoints = m_shapeGraph.getAllUnlinkPoints();

    const std::vector<Point2f> &unlinkFillTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(32, m_shapeGraph.getSpacing() * 0.1,
                                                          unlinkPoints);
    m_unlinkFills.loadTriangleData(unlinkFillTriangles, qRgb(255, 255, 255));

    const std::vector<SimpleLine> &unlinkFillPerimeters =
        GeometryGenerators::generateMultipleCircleLines(32, m_shapeGraph.getSpacing() * 0.1,
                                                        unlinkPoints);
    m_unlinkLines.loadLineData(unlinkFillPerimeters, qRgb(255, 0, 0));
}
