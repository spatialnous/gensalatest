// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.h"
#include "selfcleaningfile.h"

#include "depthmapXcli/visprepparser.h"

#include "catch_amalgamated.hpp"

#include <fstream>

TEST_CASE("VisPrepParserFail", "Error cases") {
    SECTION("Missing argument to pg") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pg", "-pp", "1.2,1.3"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-pg requires an argument"));
    }
    SECTION("Missing argument to pp") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pp"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-pp requires an argument"));
    }
    SECTION("Missing argument to pf") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pf", "-pg", "1.2"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-pf requires an argument"));
    }

    SECTION("Non-numeric input to -pg") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pg", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-pg must be a number >0, got foo"));
    }

    SECTION("rubbish input to -pp") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pp", "foo"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring(
                "Invalid fill point provided (foo). Should only contain digits dots and commas"));
    }

    SECTION("Non-existing file provide") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pf", "foo.csv"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("Failed to load file foo.csv, error"));
    }

    SECTION("Neither points nor point file provided") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pm"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Either -pp or -pf must be given"));
    }

    SECTION("Nothing to do") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Nothing to do"));
    }

    SECTION("Points and pointfile provided") {
        VisPrepParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\t2\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pp", "0.1,5.2", "-pf", "testpoints.csv"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-pf cannot be used together with -pp"));
    }

    SECTION("Pointfile and points provided") {
        VisPrepParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\t2\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pf", "testpoints.csv", "-pp", "0.1,5.2"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-pp cannot be used together with -pf"));
    }

    SECTION("Malformed pointfile") {
        VisPrepParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pf", "testpoints.csv"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Error parsing line: 1"));
    }

    SECTION("Malformed point arg") {
        VisPrepParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-pg", "0.1", "-pp", "0.1"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Error parsing line: 0.1"));
    }

    SECTION("Nonsensical visibility restriction") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pr", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "Restricted visibility of 'foo' makes no sense, use a "
                                "positive number or -1 for unrestricted"));
    }

    SECTION("Nonsensical visibility restriction") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pr", "0.0"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "Restricted visibility of '0.0' makes no sense, use a "
                                "positive number or -1 for unrestricted"));
    }

    SECTION("Make and unmake") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pm", "-pu"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-pu cannot be used together with -pm"));
    }

    SECTION("Grid and unmake") {
        VisPrepParser parser;
        ArgumentHolder ah{"prog", "-pg", "1", "-pu"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "-pu can not be used with any other option apart from -pl"));
    }
}

TEST_CASE("VisprepParserMakeSuccess", "Read successfully - Make") {
    VisPrepParser parser;
    double x1 = 1.0;
    double y1 = 2.0;
    double x2 = 1.1;
    double y2 = 1.2;
    double grid = 0.5;
    std::stringstream gstring;
    gstring << grid << std::flush;

    SECTION("Read from commandline") {
        std::stringstream p1;
        p1 << x1 << "," << y1 << std::flush;
        std::stringstream p2;
        p2 << x2 << "," << y2 << std::flush;

        ArgumentHolder ah{"prog",   "-pg", gstring.str(), "-pp", p1.str(), "-pp",
                          p2.str(), "-pb", "-pr",         "2.1", "-pm"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getBoundaryGraph());
        REQUIRE(parser.getMakeGraph());
        REQUIRE_FALSE(parser.getUnmakeGraph());
        REQUIRE_FALSE(parser.getRemoveLinksWhenUnmaking());
        REQUIRE(parser.getMaxVisibility() == Catch::Approx(2.1));
    }

    SECTION("Read from file") {
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f(scf.Filename().c_str());
            f << "x\ty\n" << x1 << "\t" << y1 << "\n" << x2 << "\t" << y2 << "\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-pg", gstring.str(), "-pf", scf.Filename()};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.getBoundaryGraph());
        REQUIRE_FALSE(parser.getMakeGraph());
        REQUIRE_FALSE(parser.getUnmakeGraph());
        REQUIRE_FALSE(parser.getRemoveLinksWhenUnmaking());
        REQUIRE(parser.getMaxVisibility() == Catch::Approx(-1.0));
    }

    REQUIRE(parser.getGrid() == Catch::Approx(grid));
    auto points = parser.getFillPoints();
    REQUIRE(points.size() == 2);
    REQUIRE(points[0].x == Catch::Approx(x1));
    REQUIRE(points[0].y == Catch::Approx(y1));
    REQUIRE(points[1].x == Catch::Approx(x2));
    REQUIRE(points[1].y == Catch::Approx(y2));
}

TEST_CASE("VisprepParserUnmakeSuccess", "Read successfully - Unmake") {
    VisPrepParser parser;
    ArgumentHolder ah{"prog", "-pu", "-pl"};
    parser.parse(ah.argc(), ah.argv());
    REQUIRE_FALSE(parser.getBoundaryGraph());
    REQUIRE_FALSE(parser.getMakeGraph());
    REQUIRE(parser.getUnmakeGraph());
    REQUIRE(parser.getRemoveLinksWhenUnmaking());
}
