// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.h"

#include "genlib/p2dpoly.h"

#include <vector>

class StepDepthParser : public IModeParser {
  public:
    StepDepthParser() : m_stepType(StepType::NONE) {}

    virtual std::string getModeName() const { return "STEPDEPTH"; }

    virtual std::string getHelp() const {
        return "Mode options for pointmap STEPDEPTH are:\n"
               "  -sdp <step depth point> point where to calculate step depth from. Can be "
               "repeated\n"
               "  -sdf <step depth point file> a file with a point per line to calculate step "
               "depth from\n"
               "  -sdt <type> step type. One of metric, angular or visual\n";
    }

    enum class StepType { NONE, ANGULAR, METRIC, VISUAL };

    virtual void parse(size_t argc, char **argv);

    virtual void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const;

    std::vector<Point2f> getStepDepthPoints() const { return m_stepDepthPoints; }

    StepType getStepType() const { return m_stepType; }

  private:
    std::vector<Point2f> m_stepDepthPoints;

    StepType m_stepType;
};
