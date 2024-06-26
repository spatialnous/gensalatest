// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "agentparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include "salalib/entityparsing.h"

#include <cstring>
#include <sstream>

using namespace depthmapX;

AgentParser::AgentParser() : m_agentMode(AgentMode::NONE) {}

void AgentParser::parse(size_t argc, char *argv[]) {
    std::vector<std::string> points;
    std::string pointFile;

    for (size_t i = 1; i < argc;) {

        if (std::strcmp("-am", argv[i]) == 0) {
            if (m_agentMode != AgentMode::NONE) {
                throw CommandLineException(
                    "-am can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-am", i)
            if (std::strcmp(argv[i], "standard") == 0) {
                m_agentMode = AgentMode::STANDARD;
            } else if (std::strcmp(argv[i], "los-length") == 0) {
                m_agentMode = AgentMode::LOS_LENGTH;
            } else if (std::strcmp(argv[i], "occ-length") == 0) {
                m_agentMode = AgentMode::OCC_LENGTH;
            } else if (std::strcmp(argv[i], "occ-any") == 0) {
                m_agentMode = AgentMode::OCC_ANY;
            } else if (std::strcmp(argv[i], "occ-group-45") == 0) {
                m_agentMode = AgentMode::OCC_GROUP_45;
            } else if (std::strcmp(argv[i], "occ-group-60") == 0) {
                m_agentMode = AgentMode::OCC_GROUP_60;
            } else if (std::strcmp(argv[i], "occ-furthest") == 0) {
                m_agentMode = AgentMode::OCC_FURTHEST;
            } else if (std::strcmp(argv[i], "bin-far-dist") == 0) {
                m_agentMode = AgentMode::BIN_FAR_DIST;
            } else if (std::strcmp(argv[i], "bin-angle") == 0) {
                m_agentMode = AgentMode::BIN_ANGLE;
            } else if (std::strcmp(argv[i], "bin-far-dist-angle") == 0) {
                m_agentMode = AgentMode::BIN_FAR_DIST_ANGLE;
            } else if (std::strcmp(argv[i], "bin-memory") == 0) {
                m_agentMode = AgentMode::BIN_MEMORY;
            } else {
                throw CommandLineException(std::string("Invalid AGENTS mode: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-ats") == 0) {
            if (m_totalSystemTimestemps > 0) {
                throw CommandLineException("-ats can only be used once");
            }
            ENFORCE_ARGUMENT("-ats", i)
            if (!has_only_digits(argv[i])) {
                throw CommandLineException(std::string("-ats must be a number >0, got ") + argv[i]);
            }
            m_totalSystemTimestemps = static_cast<size_t>(std::atoi(argv[i]));
            if (m_totalSystemTimestemps <= 0) {
                throw CommandLineException(std::string("-ats must be a number >0, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-arr") == 0) {
            if (m_releaseRate > 0) {
                throw CommandLineException("-arr can only be used once");
            }
            ENFORCE_ARGUMENT("-arr", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                throw CommandLineException(std::string("-arr must be a number >0, got ") + argv[i]);
            }
            m_releaseRate = std::atof(argv[i]);
            if (m_releaseRate <= 0) {
                throw CommandLineException(std::string("-arr must be a number >0, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-atrails") == 0) {
            if (m_recordTrailsForAgents >= 0) {
                throw CommandLineException("-atrails can only be used once");
            }
            ENFORCE_ARGUMENT("-atrails", i)
            if (!has_only_digits(argv[i])) {
                throw CommandLineException(
                    std::string(
                        "-atrails must be a number >=1 or 0 for all (max possible = 50), got ") +
                    argv[i]);
            }
            m_recordTrailsForAgents = std::atoi(argv[i]);
            if (m_recordTrailsForAgents < 0) {
                throw CommandLineException(
                    std::string(
                        "-atrails must be a number >=1 or 0 for all (max possible = 50), got ") +
                    argv[i]);
            }
        } else if (std::strcmp(argv[i], "-afov") == 0) {
            if (m_agentFOV > 0) {
                throw CommandLineException("-afov can only be used once");
            }
            ENFORCE_ARGUMENT("-afov", i)
            if (!has_only_digits(argv[i])) {
                throw CommandLineException(
                    std::string("-afov must be a number between 1 and 32, got ") + argv[i]);
            }
            m_agentFOV = std::atoi(argv[i]);
            if (m_agentFOV <= 0 || m_agentFOV > 32) {
                throw CommandLineException(
                    std::string("-afov must be a number between 1 and 32, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-asteps") == 0) {
            if (m_agentStepsBeforeTurnDecision > 0) {
                throw CommandLineException("-asteps can only be used once");
            }
            ENFORCE_ARGUMENT("-asteps", i)
            if (!has_only_digits(argv[i])) {
                throw CommandLineException(std::string("-asteps must be a number >0, got ") +
                                           argv[i]);
            }
            m_agentStepsBeforeTurnDecision = std::atoi(argv[i]);
            if (m_agentStepsBeforeTurnDecision <= 0) {
                throw CommandLineException(std::string("-asteps must be a number >0, got ") +
                                           argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alife") == 0) {
            if (m_agentLifeTimesteps > 0) {
                throw CommandLineException("-alife can only be used once");
            }
            ENFORCE_ARGUMENT("-alife", i)
            if (!has_only_digits(argv[i])) {
                throw CommandLineException(std::string("-alife must be a number >0, got ") +
                                           argv[i]);
            }
            m_agentLifeTimesteps = std::atoi(argv[i]);
            if (m_agentLifeTimesteps <= 0) {
                throw CommandLineException(std::string("-alife must be a number >0, got ") +
                                           argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alocseed") == 0) {
            if (!pointFile.empty()) {
                throw CommandLineException("-alocseed cannot be used together with -alocfile");
            }
            if (!points.empty()) {
                throw CommandLineException("-alocseed cannot be used together with -aloc");
            }
            ENFORCE_ARGUMENT("-alocseed", i)
            if (!has_only_digits(argv[i])) {
                std::stringstream message;
                message << "Invalid starting location seed provided (" << argv[i]
                        << "). Should only contain digits" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            m_randomReleaseLocationSeed = std::atoi(argv[i]);
            if (m_randomReleaseLocationSeed < 0 || m_randomReleaseLocationSeed > 10) {
                throw CommandLineException(
                    std::string("-alocseed must be a number between 0 and 10, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alocfile") == 0) {
            if (!points.empty()) {
                throw CommandLineException("-alocfile cannot be used together with -aloc");
            }
            if (m_randomReleaseLocationSeed > -1) {
                throw CommandLineException("-alocfile cannot be used together with -alocseed");
            }
            ENFORCE_ARGUMENT("-alocfile", i)
            pointFile = argv[i];
        } else if (std::strcmp(argv[i], "-aloc") == 0) {
            if (!pointFile.empty()) {
                throw CommandLineException("-aloc cannot be used together with -alocfile");
            }
            if (m_randomReleaseLocationSeed > -1) {
                throw CommandLineException("-aloc cannot be used together with -alocseed");
            }
            ENFORCE_ARGUMENT("-aloc", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid starting point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            points.push_back(argv[i]);
        } else if (std::strcmp(argv[i], "-ot") == 0) {
            ENFORCE_ARGUMENT("-ot", i)
            if (std::strcmp(argv[i], "graph") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::GRAPH) !=
                    m_outputTypes.end()) {
                    throw CommandLineException("Same output type argument (graph) provided twice");
                }
                m_outputTypes.push_back(OutputType::GRAPH);
            } else if (std::strcmp(argv[i], "gatecounts") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::GATECOUNTS) !=
                    m_outputTypes.end()) {
                    throw CommandLineException(
                        "Same output type argument (gatecounts) provided twice");
                }
                m_outputTypes.push_back(OutputType::GATECOUNTS);
            } else if (std::strcmp(argv[i], "trails") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::TRAILS) !=
                    m_outputTypes.end()) {
                    throw CommandLineException("Same output type argument (trails) provided twice");
                }
                m_outputTypes.push_back(OutputType::TRAILS);
            }
        }
        ++i;
    }

    if (m_agentMode == AgentMode::NONE) {
        m_agentMode = AgentMode::STANDARD;
    }

    if (m_totalSystemTimestemps == 0) {
        throw CommandLineException("Total number of timesteps (-ats <timesteps>) is required");
    }

    if (m_releaseRate == 0) {
        throw CommandLineException("Release rate (-arr <rate>) is required");
    }

    if (m_agentFOV == 0) {
        throw CommandLineException("Agent field-of-view (-afov <bins>) is required");
    }

    if (m_agentStepsBeforeTurnDecision == 0) {
        throw CommandLineException(
            "Agent number of steps before turn decision (-asteps <steps>) is required");
    }

    if (m_agentLifeTimesteps == 0) {
        throw CommandLineException("Agent life in timesteps (-alife <timesteps>) is required");
    }

    if (pointFile.empty() && points.empty() && m_randomReleaseLocationSeed == -1) {
        throw CommandLineException("Either -aloc, -alocfile or -alocseed must be given");
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
        m_releasePoints.insert(std::end(m_releasePoints), std::begin(parsed), std::end(parsed));
    } else if (!points.empty()) {
        std::stringstream pointsStream;
        pointsStream << "x,y";
        std::vector<std::string>::iterator iter = points.begin(), end = points.end();
        for (; iter != end; ++iter) {
            pointsStream << "\n" << *iter;
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
        m_releasePoints.insert(std::end(m_releasePoints), std::begin(parsed), std::end(parsed));
    }
}

void AgentParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::runAgentAnalysis(clp, *this, perfWriter);
}
