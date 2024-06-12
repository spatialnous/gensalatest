// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.h"

class SegmentParser : public IModeParser {
  public:
    SegmentParser();

    // IModeParser interface
  public:
    std::string getModeName() const;
    std::string getHelp() const;
    void parse(size_t argc, char **argv);
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const;

    enum class AnalysisType { NONE, ANGULAR_TULIP, ANGULAR_FULL, TOPOLOGICAL, METRIC };

    enum class RadiusType { NONE, SEGMENT_STEPS, ANGULAR, METRIC };

    AnalysisType getAnalysisType() const { return m_analysisType; }

    RadiusType getRadiusType() const { return m_radiusType; }

    bool includeChoice() const { return m_includeChoice; }

    int getTulipBins() const { return m_tulipBins; }

    const std::vector<double> getRadii() const { return m_radii; }

    const std::string getAttribute() const { return m_attribute; }

  private:
    AnalysisType m_analysisType;
    RadiusType m_radiusType;
    bool m_includeChoice;
    int m_tulipBins;
    std::vector<double> m_radii;
    std::string m_attribute;
};
