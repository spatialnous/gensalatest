// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/mapconverter.h"

#include "catch_amalgamated.hpp"
#include "salalib/shapemapgroupdata.h"

TEST_CASE("Failing empty drawing map conversion", "") {
    {
        std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles;
        auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);

        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToAxial(nullptr, "Axial map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("Failed to convert lines"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No lines found in drawing"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToConvex(nullptr, "Convex map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No polygons found in drawing"));
    }
    {
        std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);
        auto &spacePixelFileData = drawingFiles.back().first;
        spacePixelFileData.name = "Test SpacePixelGroup";
        auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);

        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToAxial(nullptr, "Axial map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("Failed to convert lines"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No lines found in drawing"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToConvex(nullptr, "Convex map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No polygons found in drawing"));
    }

    {
        std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);
        auto &spacePixelFileData = drawingFiles.back().first;
        spacePixelFileData.name = "Test SpacePixelGroup";
        drawingFiles.back().second.push_back(ShapeMap("Drawing layer", ShapeMap::DRAWINGMAP));
        auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);

        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToAxial(nullptr, "Axial map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("Failed to convert lines"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No lines found in drawing"));
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToConvex(nullptr, "Convex map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("No polygons found in drawing"));
    }
}

TEST_CASE("Failing empty axial to segment map conversion", "") {
    ShapeGraph segmentMap("Axial map", ShapeMap::AXIALMAP);
    // TODO: Does not throw an exception but maybe it should as the axial map is empty?
    // REQUIRE_THROWS_WITH(MapConverter::convertAxialToSegment(nullptr, segmentMap, "Segment map",
    // false, false, 0), Catch::Matchers::ContainsSubstring("No lines found in drawing"));
}

TEST_CASE("Failing empty data map conversion", "") {
    ShapeMap dataMap("Data map", ShapeMap::DATAMAP);
    REQUIRE_THROWS_WITH(MapConverter::convertDataToAxial(nullptr, "Axial map", dataMap),
                        Catch::Matchers::ContainsSubstring("No lines found in data map"));
    REQUIRE_THROWS_WITH(MapConverter::convertDataToSegment(nullptr, "Segment map", dataMap),
                        Catch::Matchers::ContainsSubstring("No lines found in data map"));
    REQUIRE_THROWS_WITH(MapConverter::convertDataToConvex(nullptr, "Convex map", dataMap),
                        Catch::Matchers::ContainsSubstring("No polygons found in data map"));
}

TEST_CASE("Test drawing to segment conversion", "") {
    const float EPSILON = 0.001f;

    Line line1(Point2f(0, 0), Point2f(0, 1));
    Line line2(Point2f(0, 1), Point2f(1, 1));
    Line line3(Point2f(1, 1), Point2f(1, 0));

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Drawing layer", ShapeMap::DRAWINGMAP);

    ShapeMap &drawingLayer = spacePixels.back();

    auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);

    SECTION("Single line") {
        drawingLayer.makeLineShape(line1);

        // TODO: This fails with std::bad_alloc because there's only 1 line in the drawing
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("std::bad_alloc"));
    }

    SECTION("Two lines") {
        drawingLayer.makeLineShape(line1);
        drawingLayer.makeLineShape(line2);

        // TODO: This fails with std::bad_alloc because there's only 2 lines in the drawing
        REQUIRE_THROWS_WITH(
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs),
            Catch::Matchers::ContainsSubstring("std::bad_alloc"));
    }

    SECTION("Three lines") {
        drawingLayer.makeLineShape(line1);
        drawingLayer.makeLineShape(line2);
        drawingLayer.makeLineShape(line3);
        std::unique_ptr<ShapeGraph> segmentMap =
            MapConverter::convertDrawingToSegment(nullptr, "Segment map", drawingMapRefs);
        std::map<int, SalaShape> &shapes = segmentMap->getAllShapes();
        REQUIRE(shapes.size() == 3);
        auto shapeIter = shapes.begin();
        REQUIRE(shapeIter->first == 0);
        const Line &segmentLine1 = shapeIter->second.getLine();
        REQUIRE(segmentLine1.ax() == Catch::Approx(line1.ax()).epsilon(EPSILON));
        REQUIRE(segmentLine1.ay() == Catch::Approx(line1.ay()).epsilon(EPSILON));
        REQUIRE(segmentLine1.bx() == Catch::Approx(line1.bx()).epsilon(EPSILON));
        REQUIRE(segmentLine1.by() == Catch::Approx(line1.by()).epsilon(EPSILON));
        shapeIter++;
        REQUIRE(shapeIter->first == 1);
        const Line &segmentLine2 = shapeIter->second.getLine();
        REQUIRE(segmentLine2.ax() == Catch::Approx(line2.ax()).epsilon(EPSILON));
        REQUIRE(segmentLine2.ay() == Catch::Approx(line2.ay()).epsilon(EPSILON));
        REQUIRE(segmentLine2.bx() == Catch::Approx(line2.bx()).epsilon(EPSILON));
        REQUIRE(segmentLine2.by() == Catch::Approx(line2.by()).epsilon(EPSILON));
        shapeIter++;
        REQUIRE(shapeIter->first == 2);
        const Line &segmentLine3 = shapeIter->second.getLine();
        REQUIRE(segmentLine3.ax() == Catch::Approx(line3.ax()).epsilon(EPSILON));
        REQUIRE(segmentLine3.ay() == Catch::Approx(line3.ay()).epsilon(EPSILON));
        REQUIRE(segmentLine3.bx() == Catch::Approx(line3.bx()).epsilon(EPSILON));
        REQUIRE(segmentLine3.by() == Catch::Approx(line3.by()).epsilon(EPSILON));
    }
}

TEST_CASE("Test data to segment conversion", "") {
    const float EPSILON = 0.001f;

    std::string newAttributeName = "testID";
    ShapeMap dataMap("Data map", ShapeMap::DATAMAP);
    auto newAttributeID = dataMap.addAttribute(newAttributeName);

    std::vector<Line> lines;
    std::vector<std::map<int, float>> extraAttributes;

    lines.push_back(Line(Point2f(0, 0), Point2f(0, 1)));
    lines.push_back(Line(Point2f(0, 1), Point2f(1, 1)));
    lines.push_back(Line(Point2f(1, 1), Point2f(1, 0)));

    for (size_t i = 0; i < lines.size(); i++) {
        extraAttributes.push_back(std::map<int, float>());
        extraAttributes.back()[static_cast<int>(newAttributeID)] =
            static_cast<float>(extraAttributes.size());
    }

    SECTION("Single line with extra attributes") {
        dataMap.makeLineShape(lines[0], false, false, extraAttributes[0]);

        // TODO: This fails with std::bad_alloc because there's only 1 line in the data map
        REQUIRE_THROWS_WITH(
            MapConverter::convertDataToSegment(nullptr, "Segment map", dataMap, true),
            Catch::Matchers::ContainsSubstring("std::bad_alloc"));
    }

    SECTION("Two lines with extra attributes") {
        dataMap.makeLineShape(lines[0], false, false, extraAttributes[0]);
        dataMap.makeLineShape(lines[1], false, false, extraAttributes[1]);

        // TODO: This fails with std::bad_alloc because there's only 2 lines in the data map
        REQUIRE_THROWS_WITH(
            MapConverter::convertDataToSegment(nullptr, "Segment map", dataMap, true),
            Catch::Matchers::ContainsSubstring("std::bad_alloc"));
    }

    SECTION("Three lines") {
        dataMap.makeLineShape(lines[0], false, false, extraAttributes[0]);
        dataMap.makeLineShape(lines[1], false, false, extraAttributes[1]);
        dataMap.makeLineShape(lines[2], false, false, extraAttributes[2]);
        std::unique_ptr<ShapeGraph> segmentMap =
            MapConverter::convertDataToSegment(nullptr, "Segment map", dataMap, true);
        auto segmentNewAttributeID =
            segmentMap->getAttributeTable().getColumnIndex(newAttributeName);
        std::map<int, SalaShape> &shapes = segmentMap->getAllShapes();
        REQUIRE(shapes.size() == 3);
        auto shapeIter = shapes.begin();
        for (size_t i = 0; i < lines.size(); i++) {
            auto key = static_cast<size_t>(shapeIter->first);
            REQUIRE(key == i);
            AttributeRow &row = segmentMap->getAttributeRowFromShapeIndex(key);
            REQUIRE(row.getValue(segmentNewAttributeID) ==
                    extraAttributes[i][static_cast<int>(newAttributeID)]);
            const Line &segmentLine = shapeIter->second.getLine();
            REQUIRE(segmentLine.ax() == Catch::Approx(lines[i].ax()).epsilon(EPSILON));
            REQUIRE(segmentLine.ay() == Catch::Approx(lines[i].ay()).epsilon(EPSILON));
            REQUIRE(segmentLine.bx() == Catch::Approx(lines[i].bx()).epsilon(EPSILON));
            REQUIRE(segmentLine.by() == Catch::Approx(lines[i].by()).epsilon(EPSILON));
            shapeIter++;
        }
    }

    SECTION("Four lines, second line twice") {
        dataMap.makeLineShape(lines[0], false, false, extraAttributes[0]);
        dataMap.makeLineShape(lines[1], false, false, extraAttributes[1]);
        dataMap.makeLineShape(lines[1], false, false,
                              extraAttributes[1]); // this one should be removed by tidylines
        dataMap.makeLineShape(lines[2], false, false, extraAttributes[2]);
        std::unique_ptr<ShapeGraph> segmentMap =
            MapConverter::convertDataToSegment(nullptr, "Segment map", dataMap, true);
        auto segmentNewAttributeID =
            segmentMap->getAttributeTable().getColumnIndex(newAttributeName);
        std::map<int, SalaShape> &shapes = segmentMap->getAllShapes();
        REQUIRE(shapes.size() == 3);
        auto shapeIter = shapes.begin();
        for (size_t i = 0; i < lines.size(); i++) {
            auto key = static_cast<size_t>(shapeIter->first);
            REQUIRE(key == i);
            AttributeRow &row = segmentMap->getAttributeRowFromShapeIndex(key);
            REQUIRE(row.getValue(segmentNewAttributeID) ==
                    extraAttributes[i][static_cast<int>(newAttributeID)]);
            const Line &segmentLine = shapeIter->second.getLine();
            REQUIRE(segmentLine.ax() == Catch::Approx(lines[i].ax()).epsilon(EPSILON));
            REQUIRE(segmentLine.ay() == Catch::Approx(lines[i].ay()).epsilon(EPSILON));
            REQUIRE(segmentLine.bx() == Catch::Approx(lines[i].bx()).epsilon(EPSILON));
            REQUIRE(segmentLine.by() == Catch::Approx(lines[i].by()).epsilon(EPSILON));
            shapeIter++;
        }
    }
}
