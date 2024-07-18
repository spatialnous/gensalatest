// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "depthmapXcli/simpletimer.h"

#include "catch_amalgamated.hpp"

#include <chrono>
#include <thread>

TEST_CASE("TestSimpleTimer", "") {
    SimpleTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE(timer.getTimeInSeconds() == Catch::Approx(0.5).epsilon(0.2));
}

TEST_CASE("TestSimpleTimerReset", "") {
    SimpleTimer timer1;
    SimpleTimer timer2;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE(timer1.getTimeInSeconds() == Catch::Approx(0.5).epsilon(0.2));
    REQUIRE(timer2.getTimeInSeconds() == Catch::Approx(0.5).epsilon(0.2));
    timer2.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE(timer1.getTimeInSeconds() == Catch::Approx(1.0).epsilon(0.2));
    REQUIRE(timer2.getTimeInSeconds() == Catch::Approx(0.5).epsilon(0.2));
}
