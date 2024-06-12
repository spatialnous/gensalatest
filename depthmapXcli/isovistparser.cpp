// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "isovistparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include "salalib/entityparsing.h"

#include <cstring>
#include <sstream>

using namespace depthmapX;

IsovistParser::IsovistParser() {}

std::string IsovistParser::getModeName() const { return "ISOVIST"; }

std::string IsovistParser::getHelp() const {
    return "Arguments for isovist mode:\n"
           "  -ii <x,y[,angle,viewangle]> Define an isoivist at position x,y with\n"
           "    optional direction angle and view angle for partial isovists\n"
           "  -if <isovist file> load isovist definitions from a file (csv)\n"
           "    the relevant headers must be called x, y, angle and viewangle\n"
           "    the latter two are optional.\n"
           "  Those two arguments cannot be mixed\n"
           "  Angles for partial isovists are in degrees, counted anti-clockwise with 0°\n"
           "  pointing to the right.\n\n";
}

void IsovistParser::parse(size_t argc, char **argv) {
    std::string isovistFile;

    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-ii") == 0) {
            if (!isovistFile.empty()) {
                throw CommandLineException("-ii cannot be used together with -if");
            }
            ENFORCE_ARGUMENT("-ii", i);
            m_isovists.push_back(EntityParsing::parseIsovist(argv[i]));
        } else if (std::strcmp(argv[i], "-if") == 0) {
            if (!isovistFile.empty()) {
                throw CommandLineException("-if can only be used once");
            }
            if (!m_isovists.empty()) {
                throw depthmapX::CommandLineException("-if cannot be used together with -ii");
            }
            ENFORCE_ARGUMENT("-if", i);
            isovistFile = argv[i];
        }
    }

    if (!isovistFile.empty()) {
        std::ifstream file(isovistFile);
        if (!file.good()) {
            std::stringstream message;
            message << "Failed to find file " << isovistFile;
            throw depthmapX::CommandLineException(message.str());
        }
        m_isovists = EntityParsing::parseIsovists(file, ',');
    }
    if (m_isovists.empty()) {
        throw CommandLineException("No isovists defined. Use -ii or -if");
    }
}

void IsovistParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::runIsovists(clp, m_isovists, perfWriter);
}
