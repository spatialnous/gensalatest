// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "linkparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include <cstring>
#include <memory>
#include <sstream>

using namespace depthmapX;

void LinkParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc;) {
        if (std::strcmp("-lmt", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-lmt", i)
            if (std::strcmp(argv[i], "pointmaps") == 0) {
                m_mapTypeGroup = MapTypeGroup::POINTMAPS;
            } else if (std::strcmp(argv[i], "shapegraphs") == 0) {
                m_mapTypeGroup = MapTypeGroup::SHAPEGRAPHS;
            } else {
                throw CommandLineException(std::string("Invalid LINK map type group: ") + argv[i]);
            }
        } else if (std::strcmp("-lm", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-lm", i)
            if (std::strcmp(argv[i], "link") == 0) {
                m_linkMode = LinkMode::LINK;
            } else if (std::strcmp(argv[i], "unlink") == 0) {
                m_linkMode = LinkMode::UNLINK;
            } else {
                throw CommandLineException(std::string("Invalid LINK mode: ") + argv[i]);
            }
        } else if (std::strcmp("-lt", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-lt", i)
            if (std::strcmp(argv[i], "coords") == 0) {
                m_linkType = LinkType::COORDS;
            } else if (std::strcmp(argv[i], "refs") == 0) {
                m_linkType = LinkType::REFS;
            } else {
                throw CommandLineException(std::string("Invalid LINK type: ") + argv[i]);
            }
        } else if (std::strcmp("-lf", argv[i]) == 0) {
            if (!m_linksFile.empty()) {
                throw CommandLineException("-lf can only be used once at the moment");
            } else if (m_manualLinks.size() != 0) {
                throw CommandLineException("-lf can not be used in conjunction with -lnk");
            }
            ENFORCE_ARGUMENT("-lf", i)
            m_linksFile = argv[i];
        } else if (std::strcmp("-lnk", argv[i]) == 0) {
            if (!m_linksFile.empty()) {
                throw CommandLineException("-lf can not be used in conjunction with -lnk");
            }
            ENFORCE_ARGUMENT("-lnk", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid link provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            m_manualLinks.push_back(argv[i]);
        }
        ++i;
    }
    if (m_manualLinks.size() == 0 && m_linksFile.empty()) {
        throw CommandLineException("one of -lf or -lnk must be provided");
    }
}

void LinkParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::linkGraph(clp, *this, perfWriter);
}
