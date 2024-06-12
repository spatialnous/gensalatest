// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "importparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"

#include <cstring>
#include <memory>
#include <sstream>

using namespace depthmapX;

void ImportParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc; ++i) {
        if (strcmp("-if", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-if", i)
            m_filesToImport.push_back(argv[i]);
        } else if (std::strcmp("-it", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-it", i)
            if (std::strcmp(argv[i], "drawing") == 0) {
                m_importMapType = depthmapX::ImportType::DRAWINGMAP;
            } else if (std::strcmp(argv[i], "data") == 0) {
                m_importMapType = depthmapX::ImportType::DATAMAP;
            } else {
                throw CommandLineException(std::string("Invalid map import (-it) type: ") +
                                           argv[i]);
            }
        } else if (strcmp("-iaa", argv[i]) == 0) {
            m_importAsAttributes = true;
        }
    }
}

void ImportParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    dm_runmethods::importFiles(clp, *this, perfWriter);
}
