// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "imodeparser.h"

#include <string>

class ExportParser : public IModeParser {
  public:
    virtual std::string getModeName() const { return "EXPORT"; }

    virtual std::string getHelp() const {
        return "Mode options for EXPORT:\n"
               "-em <export mode> one of:\n"
               "    pointmap-data-csv\n"
               "    pointmap-connections-csv\n"
               "    pointmap-links-csv\n"
               "    shapegraph-map-csv\n"
               "    shapegraph-map-mif\n"
               "    shapegraph-connections-csv\n"
               "    shapegraph-links-unlinks-csv\n";
    }

  public:
    ExportParser();
    virtual void parse(size_t argc, char *argv[]);
    virtual void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const;

    enum ExportMode {
        NONE,
        POINTMAP_DATA_CSV,
        POINTMAP_CONNECTIONS_CSV,
        POINTMAP_LINKS_CSV,
        SHAPEGRAPH_MAP_CSV,
        SHAPEGRAPH_MAP_MIF,
        SHAPEGRAPH_CONNECTIONS_CSV,
        SHAPEGRAPH_LINKS_UNLINKS_CSV
    };
    ExportMode getExportMode() const { return m_exportMode; }

  private:
    ExportMode m_exportMode;
};
