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
