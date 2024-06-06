// Copyright (C) 2017 Christian Sailer
// Copyright (C) 2017 Petros Koutsolampros

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

#pragma once

#include "agentparser.h"
#include "axialparser.h"
#include "commandlineparser.h"
#include "exportparser.h"
#include "importparser.h"
#include "linkparser.h"
#include "mapconvertparser.h"
#include "performancesink.h"
#include "radiusconverter.h"
#include "salalib/isovistdef.h"
#include "salalib/mgraph.h"
#include "segmentparser.h"
#include "stepdepthparser.h"
#include "vgaparser.h"

#include <string>
#include <vector>

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#define DO_TIMED(message, code)                                                                    \
    SimpleTimer CONCAT(t_, __LINE__);                                                              \
    code;                                                                                          \
    perfWriter.addData(message, CONCAT(t_, __LINE__).getTimeInSeconds());

class Line;
class Point2f;

namespace dm_runmethods {
    std::unique_ptr<MetaGraph> loadGraph(const std::string &filename, IPerformanceSink &perfWriter);
    void importFiles(const CommandLineParser &cmdP, const ImportParser &parser,
                     IPerformanceSink &perfWriter);
    void linkGraph(const CommandLineParser &cmdP, const LinkParser &parser,
                   IPerformanceSink &perfWriter);
    void runVga(const CommandLineParser &cmdP, const VgaParser &vgaP,
                const IRadiusConverter &converter, IPerformanceSink &perfWriter);
    void runVisualPrep(const CommandLineParser &clp, double gridSize,
                       const std::vector<Point2f> &fillPoints, double maxVisibility,
                       bool boundaryGraph, bool makeGraph, bool unmakeGraph,
                       bool removeLinksWhenUnmaking, IPerformanceSink &perfWriter);
    void runAxialAnalysis(const CommandLineParser &clp, const AxialParser &ap,
                          IPerformanceSink &perfWriter);
    void runSegmentAnalysis(const CommandLineParser &clp, const SegmentParser &sp,
                            IPerformanceSink &perfWriter);
    void runAgentAnalysis(const CommandLineParser &cmdP, const AgentParser &agentP,
                          IPerformanceSink &perfWriter);
    void runIsovists(const CommandLineParser &cmdP, const std::vector<IsovistDefinition> &isovists,
                     IPerformanceSink &perfWriter);
    void exportData(const CommandLineParser &cmdP, const ExportParser &exportP,
                    IPerformanceSink &perfWriter);
    void runStepDepth(const CommandLineParser &clp, const StepDepthParser::StepType &stepType,
                      const std::vector<Point2f> &stepDepthPoints, IPerformanceSink &perfWriter);
    void runMapConversion(const CommandLineParser &clp, const MapConvertParser &mcp,
                          IPerformanceSink &perfWriter);
} // namespace dm_runmethods
