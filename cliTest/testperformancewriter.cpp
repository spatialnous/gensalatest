// Copyright (C) 2017, Christian Sailer

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "selfcleaningfile.h"

#include "depthmapXcli/performancewriter.h"

#include "catch.hpp"

#include <fstream>

TEST_CASE("TestPerformanceWriting", "Simple test case") {
    SelfCleaningFile scf("timertest.csv");
    PerformanceWriter writer(scf.Filename());

    writer.addData("test1", 100.0);
    writer.addData("test2", 200.0);

    writer.write();

    std::ifstream f(scf.Filename());
    REQUIRE(f.good());
    char line[1000];
    std::vector<std::string> lines;
    while (!f.eof()) {
        f.getline(line, 1000);
        lines.push_back(line);
    }
    std::vector<std::string> expected{"\"action\",\"duration\"", "\"test1\",100", "\"test2\",200",
                                      ""};
    REQUIRE(lines == expected);
}

TEST_CASE("TestPerformanceNotWriting", "No filename no writing") {
    SelfCleaningFile scf("timertest.csv");
    PerformanceWriter writer("");

    writer.addData("test1", 100.0);
    writer.addData("test2", 200.0);

    writer.write();

    std::ifstream f(scf.Filename());
    REQUIRE_FALSE(f.good());
}
