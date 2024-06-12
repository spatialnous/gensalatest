// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "radiusconverter.h"
#include "exceptions.h"

#include <cmath>
#include <cstdlib>
#include <sstream>

using namespace depthmapX;

double RadiusConverter::ConvertForVisibility(const std::string &radius) const {
    if (radius == "n") {
        return -1.0;
    }
    char *end;
    long rad = std::strtol(radius.c_str(), &end, 10);
    if (rad < 1 || rad > 99) {
        throw SetupCheckException(
            std::string("Radius for visibility analysis must be n for the whole range or an "
                        "integer between 1 and 99 inclusive. Got ") +
            radius);
    }
    return static_cast<double>(rad);
}

double RadiusConverter::ConvertForMetric(const std::string &radius) const {
    if (radius == "n") {
        return -1.0;
    }
    char *end;
    double rad = strtod(radius.c_str(), &end);
    if (rad <= 0) {
        throw SetupCheckException(
            std::string(
                "Radius for metric vga must be n for the whole range or a positive number. Got ") +
            radius);
    }
    if (std::isnan(rad)) {
        throw SetupCheckException("Radius NaN?! Really?");
    }
    if (std::isinf(rad)) {
        throw SetupCheckException("Radius inf?! Who are you kidding?");
    }

    return rad;
}
