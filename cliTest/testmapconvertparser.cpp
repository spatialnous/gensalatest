// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.h"

#include "depthmapXcli/mapconvertparser.h"

#include "catch.hpp"

TEST_CASE("MapConvertParserFail", "Error cases") {
    SECTION("Missing argument to co") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-co"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-co requires an argument"));
    }
    SECTION("Missing argument to con") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-con"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-con requires an argument"));
    }
    SECTION("Missing argument to crsl") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-crsl"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-crsl requires an argument"));
    }

    SECTION("Non-numeric input to -crsl") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-crsl", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-crsl must be a number >0, got foo"));
    }

    SECTION("Under-zero input to -crsl") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-crsl", "-1"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-crsl must be a number >0, got -1"));
    }

    SECTION("Zero input to -crsl") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-crsl", "-1"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-crsl must be a number >0, got -1"));
    }

    SECTION("rubbish input to -co") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-co", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("Invalid map output (-co) type: foo"));
    }

    SECTION("output type (-co) provided twice") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-co", "axial", "-co", "drawing"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Contains("-co can only be used once, modes are mutually exclusive"));
    }

    SECTION("Don't provide output type") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("A valid output map type (-co) is required"));
    }

    SECTION("Don't provide output name") {
        MapConvertParser parser;
        ArgumentHolder ah{"prog", "-co", "axial"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("A valid output map name (-con) is required"));
    }
}

TEST_CASE("MapConvertParserSuccess", "Read successfully") {
    MapConvertParser parser;

    SECTION("Plain axial") {
        ArgumentHolder ah{"prog", "-co", "axial", "-con", "new_axial"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.outputMapName() == "new_axial");
        REQUIRE(parser.outputMapType() == ShapeMap::AXIALMAP);
    }
}
