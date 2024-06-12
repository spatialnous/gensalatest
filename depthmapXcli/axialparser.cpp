// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "axialparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include "salalib/entityparsing.h"

#include <cstring>

using namespace depthmapX;

AxialParser::AxialParser()
    : m_runFewestLines(false), m_runAnalysis(false), m_choice(false), m_local(false), m_rra(false) {

}

std::string AxialParser::getModeName() const { return "AXIAL"; }

std::string AxialParser::getHelp() const {
    return "Mode options for Axial Analysis:\n"
           "  -xl <x>,<y> Calculate all lines map from this seed point (can be used more than "
           "once)\n"
           "  -xf Calculate fewest lines map from all lines map\n"
           "  -xa <radius/list of radii> run axial anlysis with specified radii\n"
           " All modes expect to find the required input in the in graph\n"
           " Any combination of flags above can be specified, they will always be run in the order "
           "-aa -af -au -ax\n"
           " Further flags for axial analysis are:\n"
           "   -xac Include choice (betweenness)\n"
           "   -xal Include local measures\n"
           "   -xar Include RA, RRA and total depth\n"
           "   -xaw <map attribute name> perform weighted analysis using this attribute\n"
           "\n";
}

void AxialParser::parse(size_t argc, char **argv) {
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-xl") == 0) {
            ENFORCE_ARGUMENT("-xl", i)
            m_allAxesRoots.push_back(EntityParsing::parsePoint(argv[i]));
        } else if (std::strcmp(argv[i], "-xf") == 0) {
            m_runFewestLines = true;
        } else if (std::strcmp(argv[i], "-xa") == 0) {
            ENFORCE_ARGUMENT("-xa", i)
            if (m_runAnalysis) {
                throw CommandLineException("-xa can only be used once");
            }
            m_radii = depthmapX::parseRadiusList(argv[i]);
            m_runAnalysis = true;
        } else if (std::strcmp(argv[i], "-xal") == 0) {
            m_local = true;
        } else if (std::strcmp(argv[i], "-xac") == 0) {
            m_choice = true;
        } else if (std::strcmp(argv[i], "-xar") == 0) {
            m_rra = true;
        } else if (std::strcmp(argv[i], "-xaw") == 0) {
            ENFORCE_ARGUMENT("-xaw", i)
            m_attribute = argv[i];
        }
    }

    if (!runAllLines() && !runFewestLines() && !runUnlink() && !runAnalysis()) {
        throw CommandLineException("No axial analysis mode present");
    }
}

void AxialParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::runAxialAnalysis(clp, *this, perfWriter);
}
