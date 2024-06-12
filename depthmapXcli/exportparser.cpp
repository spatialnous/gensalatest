// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "exportparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include <cstring>
#include <sstream>

using namespace depthmapX;

ExportParser::ExportParser() : m_exportMode(ExportMode::NONE) {}

void ExportParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc;) {

        if (std::strcmp("-em", argv[i]) == 0) {
            if (m_exportMode != ExportParser::NONE) {
                throw CommandLineException(
                    "-em can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-em", i)
            if (std::strcmp(argv[i], "pointmap-data-csv") == 0) {
                m_exportMode = ExportMode::POINTMAP_DATA_CSV;
            } else if (std::strcmp(argv[i], "pointmap-connections-csv") == 0) {
                m_exportMode = ExportMode::POINTMAP_CONNECTIONS_CSV;
            } else if (std::strcmp(argv[i], "pointmap-links-csv") == 0) {
                m_exportMode = ExportMode::POINTMAP_LINKS_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-map-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_MAP_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-map-mif") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_MAP_MIF;
            } else if (std::strcmp(argv[i], "shapegraph-connections-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_CONNECTIONS_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-links-unlinks-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_LINKS_UNLINKS_CSV;
            } else {
                throw CommandLineException(std::string("Invalid EXPORT mode: ") + argv[i]);
            }
        }
        ++i;
    }
}

void ExportParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::exportData(clp, *this, perfWriter);
}
