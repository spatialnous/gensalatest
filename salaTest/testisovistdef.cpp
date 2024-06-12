// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/isovistdef.h"

#include "catch.hpp"

TEST_CASE("Full Isovist") {
    IsovistDefinition isovist(1.0, 1.0);
    REQUIRE(isovist.getLocation().x == Approx(1.0));
    REQUIRE(isovist.getLocation().y == Approx(1.0));
    REQUIRE(isovist.getAngle() == 0.0);
    REQUIRE(isovist.getViewAngle() == 0.0);
}

TEST_CASE("Partial Isovist") {
    SECTION("No overrun") {
        IsovistDefinition isovist(1.0, 1.0, 2.0, 1.0);
        REQUIRE(isovist.getLocation().x == Approx(1.0));
        REQUIRE(isovist.getLocation().y == Approx(1.0));
        REQUIRE(isovist.getAngle() == Approx(2.0));
        REQUIRE(isovist.getViewAngle() == Approx(1.0));
        REQUIRE(isovist.getLeftAngle() == Approx(1.5));
        REQUIRE(isovist.getRightAngle() == Approx(2.5));
    }

    SECTION("Overrun left") {
        IsovistDefinition isovist(1.0, 1.0, 0.1, 1.0);
        REQUIRE(isovist.getLocation().x == Approx(1.0));
        REQUIRE(isovist.getLocation().y == Approx(1.0));
        REQUIRE(isovist.getAngle() == Approx(0.1));
        REQUIRE(isovist.getViewAngle() == Approx(1.0));
        REQUIRE(isovist.getLeftAngle() == Approx(5.8831853072));
        REQUIRE(isovist.getRightAngle() == Approx(0.6));
    }

    SECTION("Overrun right") {
        IsovistDefinition isovist(1.0, 1.0, 6.1, 1.0);
        REQUIRE(isovist.getLocation().x == Approx(1.0));
        REQUIRE(isovist.getLocation().y == Approx(1.0));
        REQUIRE(isovist.getAngle() == Approx(6.1));
        REQUIRE(isovist.getViewAngle() == Approx(1.0));
        REQUIRE(isovist.getLeftAngle() == Approx(5.6));
        REQUIRE(isovist.getRightAngle() == Approx(0.31681469));
    }

    SECTION("Actually a full isovist in a partial list") {
        IsovistDefinition isovist(1.0, 1.0, 6.1, 2 * M_PI);
        REQUIRE(isovist.getLocation().x == Approx(1.0));
        REQUIRE(isovist.getLocation().y == Approx(1.0));
        REQUIRE(isovist.getAngle() == 0.0);
        REQUIRE(isovist.getViewAngle() == 0.0);
        REQUIRE(isovist.getLeftAngle() == 0.0);
        REQUIRE(isovist.getRightAngle() == 0.0);
    }
}
