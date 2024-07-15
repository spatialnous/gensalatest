// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "exportparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

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

    auto mgraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    switch (getExportMode()) {
    case ExportParser::POINTMAP_DATA_CSV: {
        auto &currentMap = mgraph.getDisplayedPointMap();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing pointmap data", currentMap.getInternalMap().outputSummary(stream, ','))
        stream.close();
        break;
    }
    case ExportParser::POINTMAP_CONNECTIONS_CSV: {
        auto &currentMap = mgraph.getDisplayedPointMap();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing pointmap connections",
                 currentMap.getInternalMap().outputConnectionsAsCSV(stream, ","))
        stream.close();
        break;
    }
    case ExportParser::POINTMAP_LINKS_CSV: {
        auto &currentMap = mgraph.getDisplayedPointMap();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing pointmap connections",
                 currentMap.getInternalMap().outputLinksAsCSV(stream, ","))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_MAP_CSV: {
        auto &currentMap = mgraph.getDisplayedShapeGraph();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing pointmap connections", currentMap.getInternalMap().output(stream, ','))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_MAP_MIF: {
        auto &currentMap = mgraph.getDisplayedShapeGraph();
        std::string fileName = clp.getOuputFile().c_str();
        std::string mifFile = fileName + ".mif";
        std::string midFile = fileName + ".mid";
        if (0 == fileName.compare(fileName.length() - 4, 4, ".mif")) {
            // we are given the .mif
            mifFile = fileName;
            midFile = fileName.substr(0, fileName.length() - 4) + ".mid";

        } else if (0 == fileName.compare(fileName.length() - 4, 4, ".mid")) {
            // we are given the .mid
            mifFile = fileName.substr(0, fileName.length() - 4) + ".mif";
            midFile = fileName;
        }
        std::ofstream mifStream(mifFile);
        std::ofstream midStream(midFile);
        DO_TIMED("Writing pointmap connections",
                 currentMap.getInternalMap().outputMifMap(mifStream, midStream))
        mifStream.close();
        midStream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_CONNECTIONS_CSV: {
        auto &currentMap = mgraph.getDisplayedShapeGraph();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing shapegraph connections",
                 currentMap.getInternalMap().isAxialMap()
                     ? currentMap.getInternalMap().writeAxialConnectionsAsPairsCSV(stream)
                     : currentMap.getInternalMap().writeSegmentConnectionsAsPairsCSV(stream))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_LINKS_UNLINKS_CSV: {
        auto &currentMap = mgraph.getDisplayedShapeGraph();
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing shapegraph links and unlinks",
                 currentMap.getInternalMap().writeLinksUnlinksAsPairsCSV(stream))
        stream.close();
        break;
    }
    default: {
        throw depthmapX::SetupCheckException("Error, unsupported export mode");
    }
    }
}
