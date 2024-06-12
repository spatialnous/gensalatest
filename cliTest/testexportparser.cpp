// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.h"

#include "depthmapXcli/exportparser.h"

#include "catch.hpp"

TEST_CASE("ExportParser Fail", "Parsing errors") {
    // missing arguments

    SECTION("Missing argument to -em") {
        ExportParser parser;
        ArgumentHolder ah{"prog", "-em"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Contains("-em requires an argument"));
    }
}

TEST_CASE("ExportParser Success", "Read successfully") {
    ExportParser parser;

    SECTION("Correctly parse mode pointmap-connections-csv") {
        ArgumentHolder ah{"prog", "-em", "pointmap-connections-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::POINTMAP_CONNECTIONS_CSV);
    }

    SECTION("Correctly parse mode pointmap-data-csv") {
        ArgumentHolder ah{"prog", "-em", "pointmap-data-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::POINTMAP_DATA_CSV);
    }

    SECTION("Correctly parse mode pointmap-links-csv") {
        ArgumentHolder ah{"prog", "-em", "pointmap-links-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::POINTMAP_LINKS_CSV);
    }
}
