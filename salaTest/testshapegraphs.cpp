// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/mapconverter.h"
#include "salalib/mgraph.h"
#include "salalib/shapegraph.h"
#include "salalib/shapemap.h"

#include "genlib/p2dpoly.h"

#include "catch.hpp"

#include <iostream>
#include <sstream>

TEST_CASE("Testing ShapeGraph::writeAxialConnections") {

    Point2f line1Start(0, 0);
    Point2f line1End(3, 0);
    Point2f line2Start(1, 1);
    Point2f line2End(1, -1);
    Point2f line3Start(2, 1);
    Point2f line3End(2, -2);

    std::unique_ptr<MetaGraph> metaGraph(new MetaGraph("Test MetaGraph"));

    metaGraph->m_drawingFiles.emplace_back("Test SpacePixelGroup");
    metaGraph->m_drawingFiles.back().m_spacePixels.emplace_back("Test ShapeMap");

    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line1Start, line1End));
    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line2Start, line2End));
    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line3Start, line3End));

    auto shapegraph =
        MapConverter::convertDrawingToAxial(0, "Test axial", metaGraph->m_drawingFiles);

    SECTION("writeAxialConnectionsAsDotGraph") {
        std::stringstream stream;
        shapegraph->writeAxialConnectionsAsDotGraph(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{
            "strict graph {", "    0 -- 1", "    0 -- 2", "    1 -- 0", "    2 -- 0", "}", ""};
        REQUIRE(lines == expected);
    }
    SECTION("writeAxialConnectionsAsPairsCSV") {
        std::stringstream stream;
        shapegraph->writeAxialConnectionsAsPairsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{"refA,refB", "0,1", "0,2", "1,0", "2,0"};
        REQUIRE(lines == expected);
    }
}
TEST_CASE("Testing ShapeGraph::writeSegmentConnections") {
    // As we are converting the drawing directly to segments
    // the lines need to touch, not cross

    Point2f line1Start(1, 1);
    Point2f line1End(1, 0);
    Point2f line2Start(1, 0);
    Point2f line2End(2, 0);
    Point2f line3Start(2, 0);
    Point2f line3End(2, 2);

    std::unique_ptr<MetaGraph> metaGraph(new MetaGraph("Test MetaGraph"));

    metaGraph->m_drawingFiles.emplace_back("Test SpacePixelGroup");
    metaGraph->m_drawingFiles.back().m_spacePixels.emplace_back("Test ShapeMap");

    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line1Start, line1End));
    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line2Start, line2End));
    metaGraph->m_drawingFiles.back().m_spacePixels.back().makeLineShape(Line(line3Start, line3End));

    auto shapegraph =
        MapConverter::convertDrawingToSegment(0, "Test segment", metaGraph->m_drawingFiles);

    SECTION("writeSegmentConnectionsAsPairsCSV") {
        std::stringstream stream;
        shapegraph->writeSegmentConnectionsAsPairsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{"refA,refB,ss_weight,for_back,dir", "0,1,1,0,1",
                                          "1,2,1,0,1", "1,0,1,1,-1", "2,1,1,1,-1"};
        REQUIRE(lines == expected);
    }
}

// While the linking functionality is placed in the ShapeMap,
// (for example the variables m_links and m_unlinks) it can
// only be used through ShapeGraph because it starts with
// m_hasgraph = true. Ideally the linking functionality should
// move to the ShapeGraph

TEST_CASE("Testing ShapeMap::getAllLinkLines and ShapeMap::getAllUnlinkPoints()") {
    const float EPSILON = 0.001f;
    const double TOLERANCE_A = 1e-9;
    Point2f line0Start(0.522, 0.424);
    Point2f line0End(0.709, 1.098);
    Point2f line1Start(0.897, 1.123);
    Point2f line1End(1.122, 0.421);
    Point2f line2Start(1.073, 0.386);
    Point2f line2End(1.269, 1.196);

    std::unique_ptr<ShapeGraph> shapeGraph(new ShapeGraph("Test ShapeMap"));

    shapeGraph->makeLineShape(Line(line0Start, line0End));
    shapeGraph->makeLineShape(Line(line1Start, line1End));
    shapeGraph->makeLineShape(Line(line2Start, line2End));

    shapeGraph->makeShapeConnections();

    shapeGraph->linkShapes(0, 1);
    shapeGraph->unlinkShapes(1, 2);

    std::vector<SimpleLine> linkLines = shapeGraph->getAllLinkLines();

    REQUIRE(linkLines.size() == 1);

    REQUIRE(linkLines[0].start().x == Approx((line0Start.x + line0End.x) * 0.5).epsilon(EPSILON));
    REQUIRE(linkLines[0].start().y == Approx((line0Start.y + line0End.y) * 0.5).epsilon(EPSILON));
    REQUIRE(linkLines[0].end().x == Approx((line1Start.x + line1End.x) * 0.5).epsilon(EPSILON));
    REQUIRE(linkLines[0].end().y == Approx((line1Start.y + line1End.y) * 0.5).epsilon(EPSILON));

    std::vector<Point2f> unlinkPoints = shapeGraph->getAllUnlinkPoints();

    REQUIRE(unlinkPoints.size() == 1);

    Point2f intersection =
        intersection_point(Line(line1Start, line1End), Line(line2Start, line2End), TOLERANCE_A);

    REQUIRE(unlinkPoints[0].x == Approx(intersection.x).epsilon(EPSILON));
    REQUIRE(unlinkPoints[0].y == Approx(intersection.y).epsilon(EPSILON));
}
