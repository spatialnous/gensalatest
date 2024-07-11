// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "segmentshortestpathparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

#include "salalib/entityparsing.h"
#include "salalib/segmmodules/segmmetricshortestpath.h"
#include "salalib/segmmodules/segmtopologicalshortestpath.h"
#include "salalib/segmmodules/segmtulipshortestpath.h"

#include <cstring>
#include <sstream>

using namespace depthmapX;

void SegmentShortestPathParser::parse(size_t argc, char **argv) {

    std::string originPoint;
    std::string destinationPoint;
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-sspo", argv[i]) == 0) {
            if (!originPoint.empty()) {
                throw CommandLineException("-sspo can only be provided once");
            }
            ENFORCE_ARGUMENT("-sspo", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid origin point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            originPoint = argv[i];
        }
        if (std::strcmp("-sspd", argv[i]) == 0) {
            if (!destinationPoint.empty()) {
                throw CommandLineException("-sspd can only be provided once");
            }
            ENFORCE_ARGUMENT("-sspd", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid destination point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            destinationPoint = argv[i];
        } else if (std::strcmp("-sspt", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-sspt", i)
            if (std::strcmp(argv[i], "tulip") == 0) {
                m_stepType = StepType::TULIP;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_stepType = StepType::METRIC;
            } else if (std::strcmp(argv[i], "topological") == 0) {
                m_stepType = StepType::TOPOLOGICAL;
            } else {
                throw CommandLineException(std::string("Invalid step type: ") + argv[i]);
            }
        }
    }

    if (originPoint.empty() || destinationPoint.empty()) {
        throw CommandLineException("Both -sspo and -sspd must be provided");
    }

    std::stringstream pointsStream;
    pointsStream << "x,y";
    pointsStream << "\n" << originPoint;
    pointsStream << "\n" << destinationPoint;
    std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
    m_originPoint = parsed[0];
    m_destinationPoint = parsed[1];

    if (m_stepType == StepType::NONE) {
        throw CommandLineException("Step depth type (-sspt) must be provided");
    }
}

void SegmentShortestPathParser::run(const CommandLineParser &clp,
                                    IPerformanceSink &perfWriter) const {
    auto mGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::cout << "ok\nSelecting cells... " << std::flush;

    auto graphRegion = mGraph.getRegion();

    if (!graphRegion.contains(m_originPoint)) {
        throw depthmapX::RuntimeException("Origin point outside of target region");
    }
    if (!graphRegion.contains(m_destinationPoint)) {
        throw depthmapX::RuntimeException("Destination point outside of target region");
    }
    QtRegion r(m_originPoint, m_originPoint);
    mGraph.setCurSel(r, false);

    r = QtRegion(m_destinationPoint, m_destinationPoint);
    mGraph.setCurSel(r, true);

    std::cout << "ok\nCalculating shortest path... " << std::flush;

    std::unique_ptr<Communicator> comm(new ICommunicator());
    int refFrom = *mGraph.getSelSet().begin();
    int refTo = *mGraph.getSelSet().rbegin();

    switch (m_stepType) {
    case SegmentShortestPathParser::StepType::TULIP: {
        auto &map = mGraph.getDisplayedShapeGraph();
        DO_TIMED("Calculating tulip shortest path",
                 SegmentTulipShortestPath(map.getInternalMap(), refFrom, refTo).run(comm.get()))
        break;
    }
    case SegmentShortestPathParser::StepType::METRIC: {
        auto &map = mGraph.getDisplayedShapeGraph();
        DO_TIMED("Calculating metric shortest path",
                 SegmentMetricShortestPath(map.getInternalMap(), refFrom, refTo).run(comm.get()))
        break;
    }
    case SegmentShortestPathParser::StepType::TOPOLOGICAL: {
        auto &map = mGraph.getDisplayedShapeGraph();
        DO_TIMED(
            "Calculating topological shortest path",
            SegmentTopologicalShortestPath(map.getInternalMap(), refFrom, refTo).run(comm.get()))
        break;
    }
    default: {
        throw depthmapX::SetupCheckException("Error, unsupported step type");
    }
    }

    std::cout << " ok\nWriting out result..." << std::flush;
    DO_TIMED("Writing graph", mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
    std::cout << " ok" << std::endl;
}
