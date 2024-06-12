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

#include "visprepparser.h"
#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include "salalib/entityparsing.h"

#include <cstring>
#include <sstream>

using namespace depthmapX;

void VisPrepParser::parse(size_t argc, char **argv) {

    // All options:
    // Create grid (m_grid > 0)
    // Fill grid (pointFile or points not empty)
    // Make graph
    // Unmake graph

    // All possible combinations:
    // Create Grid + Fill Grid
    // Create Grid + Fill Grid + Make graph
    // Fill Grid
    // Fill Grid + Make graph
    // Make Graph
    // Unmake Graph

    // Bad combinations:
    // Create Grid + Make graph = Creates empty graph
    // Unmake graph + Anything else = Pointless action

    std::vector<std::string> points;
    std::string pointFile;
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-pg", argv[i]) == 0) {
            if (m_grid >= 0) {
                throw CommandLineException("-pg can only be used once");
            }
            ENFORCE_ARGUMENT("-pg", i)
            m_grid = std::atof(argv[i]);
            if (m_grid <= 0) {
                throw CommandLineException(std::string("-pg must be a number >0, got ") + argv[i]);
            }
        } else if (std::strcmp("-pp", argv[i]) == 0) {
            if (!pointFile.empty()) {
                throw CommandLineException("-pp cannot be used together with -pf");
            }
            ENFORCE_ARGUMENT("-pp", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid fill point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            points.push_back(argv[i]);
        } else if (std::strcmp("-pf", argv[i]) == 0) {
            if (!points.empty()) {
                throw CommandLineException("-pf cannot be used together with -pp");
            }
            ENFORCE_ARGUMENT("-pf", i)
            pointFile = argv[i];
        } else if (std::strcmp("-pr", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-pr", i);
            m_maxVisibility = std::atof(argv[i]);
            if (m_maxVisibility == 0.0) {
                std::stringstream message;
                message << "Restricted visibility of '" << argv[i]
                        << "' makes no sense, use a positive number or -1 for unrestricted";
                throw CommandLineException(message.str());
            }
        } else if (std::strcmp("-pb", argv[i]) == 0) {
            m_boundaryGraph = true;
        } else if (std::strcmp("-pm", argv[i]) == 0) {
            if (getUnmakeGraph()) {
                throw CommandLineException("-pm cannot be used together with -pu");
            }
            m_makeGraph = true;
        } else if (std::strcmp("-pu", argv[i]) == 0) {
            if (getMakeGraph()) {
                throw CommandLineException("-pu cannot be used together with -pm");
            }
            m_unmakeGraph = true;
        } else if (std::strcmp("-pl", argv[i]) == 0) {
            m_removeLinksWhenUnmaking = true;
        }
    }

    if (!getMakeGraph() && !getUnmakeGraph() && m_grid <= 0 && pointFile.empty() &&
        points.empty()) {
        throw CommandLineException("Nothing to do");
    }

    if (m_grid > 0 && getMakeGraph() && pointFile.empty() && points.empty()) {
        throw CommandLineException("Creating a graph for an unfilled grid is not possible. "
                                   "Either -pp or -pf must be given");
    }

    if (!pointFile.empty()) {
        std::ifstream pointsStream(pointFile);
        if (!pointsStream) {
            std::stringstream message;
            message << "Failed to load file " << pointFile << ", error " << std::strerror(errno)
                    << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, '\t');
        m_fillPoints.insert(std::end(m_fillPoints), std::begin(parsed), std::end(parsed));
    } else if (!points.empty()) {
        std::stringstream pointsStream;
        pointsStream << "x,y";
        std::vector<std::string>::iterator iter = points.begin(), end = points.end();
        for (; iter != end; ++iter) {
            pointsStream << "\n" << *iter;
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
        m_fillPoints.insert(std::end(m_fillPoints), std::begin(parsed), std::end(parsed));
    }

    if (getUnmakeGraph() && (m_grid > 0 || !m_fillPoints.empty())) {
        throw CommandLineException("-pu can not be used with any other option apart from -pl");
    }

    if (m_removeLinksWhenUnmaking && !m_unmakeGraph) {
        throw CommandLineException("-pl can only be used together with -pu");
    }
}

void VisPrepParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::runVisualPrep(clp, m_grid, m_fillPoints, m_maxVisibility, m_boundaryGraph,
                                 m_makeGraph, m_unmakeGraph, m_removeLinksWhenUnmaking, perfWriter);
}
