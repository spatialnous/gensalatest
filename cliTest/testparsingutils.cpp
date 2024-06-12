// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "depthmapXcli/parsingutils.h"

#include "catch.hpp"

TEST_CASE("AxialRadiusParsing success") {
    std::string testString = "5,1,n";
    auto result = depthmapX::parseRadiusList(testString);
    std::vector<double> expectedResult = {1.0, 5.0, -1.0};
    REQUIRE(result == expectedResult);
}

TEST_CASE("AxialRadiusParsing failure") {
    REQUIRE_THROWS_WITH(depthmapX::parseRadiusList("5,1.1"),
                        Catch::Contains("Found non integer radius 1.1"));
    REQUIRE_THROWS_WITH(depthmapX::parseRadiusList("5,foo"),
                        Catch::Contains("Found either 0 or unparsable radius foo"));
    REQUIRE_THROWS_WITH(depthmapX::parseRadiusList("5,0"),
                        Catch::Contains("Found either 0 or unparsable radius 0"));
    REQUIRE_THROWS_WITH(depthmapX::parseRadiusList("5,-1"),
                        Catch::Contains("Radius must be either n or a positive integer"));
}
