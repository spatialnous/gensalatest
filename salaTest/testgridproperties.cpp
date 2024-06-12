// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/gridproperties.h"

#include "catch.hpp"

TEST_CASE("TestGridProperties", "Test the calculations of grid properties") {
    double maxDimension = 4.583;
    GridProperties gp(maxDimension);
    REQUIRE(gp.getDefault() == Approx(0.04));
    REQUIRE(gp.getMax() == Approx(0.8));
    REQUIRE(gp.getMin() == Approx(0.004));
}
