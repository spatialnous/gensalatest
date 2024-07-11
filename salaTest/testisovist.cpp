// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/isovistutils.h"
#include "salalib/salashape.h"
#include "salalib/shapemapgroupdata.h"

#include "genlib/comm.h"
#include "genlib/p2dpoly.h"

#include "catch.hpp"

TEST_CASE("Simple Isovist") {

    const float EPSILON = 0.001f;

    // simple plan for isovist. dot as the origin
    //  _ _ _ _
    // |     . |
    // |    _ _|
    // |   |
    // |_ _|

    std::vector<Line> planLines = {
        Line(Point2f(1, 1), Point2f(1, 3)), //
        Line(Point2f(1, 3), Point2f(3, 3)), //
        Line(Point2f(3, 3), Point2f(3, 2)), //
        Line(Point2f(3, 2), Point2f(2, 2)), //
        Line(Point2f(2, 2), Point2f(2, 1)), //
        Line(Point2f(2, 1), Point2f(1, 1))  //
    };

    Point2f isovistOrigin(2.5, 2.5);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);
    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");
    auto &newShapeMap = spacePixels.back();

    for (Line &line : planLines) {
        newShapeMap.makeLineShape(line);
    }

    ShapeMap isovistMap("Isovists");

    SECTION("With a communicator") {
        std::unique_ptr<Communicator> comm(new ICommunicator);
        IsovistUtils::createIsovistInMap(comm.get(), planLines, newShapeMap.getRegion(), isovistMap,
                                         isovistOrigin, 0, 0);
    }
    SECTION("Without a communicator") {
        IsovistUtils::createIsovistInMap(nullptr, planLines, newShapeMap.getRegion(), isovistMap,
                                         isovistOrigin, 0, 0);
    }

    SalaShape &isovist = isovistMap.getAllShapes().begin()->second;

    REQUIRE(isovist.isClosed());
    REQUIRE(isovist.isPolygon());

    // TODO: The current implementation generates a polygon of 12 points, potentially
    // because it takes them directly from the isovist gaps. This isovist only really
    // needs 5 points so it might make sense to run some sort of optimisation right
    // after generating the isovists

    REQUIRE(isovist.m_points.size() == 12);

    REQUIRE(isovist.m_points[0].x == Approx(3.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[0].y == Approx(3.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[1].x == Approx(2.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[1].y == Approx(3.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[2].x == Approx(1.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[2].y == Approx(3.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[3].x == Approx(1.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[3].y == Approx(3.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[4].x == Approx(1.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[4].y == Approx(1.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[5].x == Approx(2.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[5].y == Approx(2.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[6].x == Approx(2.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[6].y == Approx(2.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[7].x == Approx(2.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[7].y == Approx(2.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[8].x == Approx(3.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[8].y == Approx(2.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[9].x == Approx(3.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[9].y == Approx(2.0).epsilon(EPSILON));

    REQUIRE(isovist.m_points[10].x == Approx(3.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[10].y == Approx(2.5).epsilon(EPSILON));

    REQUIRE(isovist.m_points[11].x == Approx(3.0).epsilon(EPSILON));
    REQUIRE(isovist.m_points[11].y == Approx(2.5).epsilon(EPSILON));
}
