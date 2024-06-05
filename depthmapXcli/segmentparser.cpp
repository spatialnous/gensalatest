// Copyright (C) 2018 Petros Koutsolampros

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

#include "segmentparser.h"
#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "salalib/entityparsing.h"
#include <cstring>

using namespace depthmapX;

SegmentParser::SegmentParser()
    : m_analysisType(AnalysisType::NONE), m_radiusType(RadiusType::NONE), m_includeChoice(false),
      m_tulipBins(0) {}

std::string SegmentParser::getModeName() const { return "SEGMENT"; }

std::string SegmentParser::getHelp() const {
    return "Mode options for Segment Analysis:\n"
           "  -st  <type of analysis> one of:\n"
           "       tulip (Angular Tulip - Faster)\n"
           "       angular (Angular Full - Slower)\n"
           "       topological\n"
           "       metric\n"
           "  -sr  <radius/list of radii>\n"
           "  -srt <radius type> (only for Tulip) one of:\n"
           "       steps\n"
           "       metric\n"
           "       angular\n"
           "  -sic to include choice (only for Tulip)\n"
           "  -stb <tulip bins> (4 to 1024, 1024 approximates full angular)\n"
           "  -swa <map attribute name> perform weighted analysis using this attribute (only for "
           "Tulip)\n";
}

void SegmentParser::parse(size_t argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp("-st", argv[i]) == 0) {
            if (m_analysisType != AnalysisType::NONE) {
                throw CommandLineException(
                    "-st can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-st", i)
            if (std::strcmp(argv[i], "tulip") == 0) {
                m_analysisType = AnalysisType::ANGULAR_TULIP;
            } else if (std::strcmp(argv[i], "angular") == 0) {
                m_analysisType = AnalysisType::ANGULAR_FULL;
            } else if (std::strcmp(argv[i], "topological") == 0) {
                m_analysisType = AnalysisType::TOPOLOGICAL;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_analysisType = AnalysisType::METRIC;
            } else {
                throw CommandLineException(std::string("Invalid SEGMENT mode: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-srt") == 0) {
            if (m_radiusType != RadiusType::NONE) {
                throw CommandLineException(
                    "-srt can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-srt", i)
            if (std::strcmp(argv[i], "steps") == 0) {
                m_radiusType = RadiusType::SEGMENT_STEPS;
            } else if (std::strcmp(argv[i], "angular") == 0) {
                m_radiusType = RadiusType::ANGULAR;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_radiusType = RadiusType::METRIC;
            } else {
                throw CommandLineException(std::string("Invalid SEGMENT radius type: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-sic") == 0) {
            m_includeChoice = true;
        } else if (std::strcmp(argv[i], "-sr") == 0) {
            ENFORCE_ARGUMENT("-sr", i)
            m_radii = depthmapX::parseRadiusList(argv[i]);
        } else if (std::strcmp(argv[i], "-stb") == 0) {
            ENFORCE_ARGUMENT("-stb", i)

            if (!has_only_digits(argv[i])) {
                throw CommandLineException(
                    std::string("-stb must be a number between 4 and 1024, got ") + argv[i]);
            }
            m_tulipBins = std::atoi(argv[i]);
            if (m_tulipBins < 4 || m_tulipBins > 1024) {
                throw CommandLineException(
                    std::string("-stb must be a number between 4 and 1024, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-swa") == 0) {
            ENFORCE_ARGUMENT("-swa", i)
            m_attribute = argv[i];
        }
    }

    if (getAnalysisType() == AnalysisType::NONE) {
        throw CommandLineException("No analysis type given");
    }

    if (getRadii().empty()) {
        throw CommandLineException("At least one radius must be provided");
    }

    if (getAnalysisType() == AnalysisType::ANGULAR_TULIP && getRadiusType() == RadiusType::NONE) {
        throw CommandLineException("Radius type is required for tulip analysis");
    }

    if (getAnalysisType() == AnalysisType::ANGULAR_TULIP && getTulipBins() == 0) {
        throw CommandLineException("Tulip bins are required for tulip analysis");
    }

    if (getAnalysisType() != AnalysisType::ANGULAR_TULIP &&
        (getTulipBins() != 0 || getRadiusType() != RadiusType::NONE || m_includeChoice)) {
        throw CommandLineException("-stb, -srt and -sic can only be used with tulip analysis");
    }
}

void SegmentParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::runSegmentAnalysis(clp, *this, perfWriter);
}
