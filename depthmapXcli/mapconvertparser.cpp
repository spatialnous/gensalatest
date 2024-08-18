// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mapconvertparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

#include <cstring>

using namespace depthmapX;

void MapConvertParser::parse(size_t argc, char **argv) {
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-co", argv[i]) == 0) {
            if (m_outMapType != ShapeMap::EMPTYMAP) {
                throw CommandLineException(
                    "-co can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-co", i)
            if (std::strcmp(argv[i], "drawing") == 0) {
                m_outMapType = ShapeMap::DRAWINGMAP;
            } else if (std::strcmp(argv[i], "axial") == 0) {
                m_outMapType = ShapeMap::AXIALMAP;
            } else if (std::strcmp(argv[i], "segment") == 0) {
                m_outMapType = ShapeMap::SEGMENTMAP;
            } else if (std::strcmp(argv[i], "data") == 0) {
                m_outMapType = ShapeMap::DATAMAP;
            } else if (std::strcmp(argv[i], "convex") == 0) {
                m_outMapType = ShapeMap::CONVEXMAP;
            } else {
                throw CommandLineException(std::string("Invalid map output (-co) type: ") +
                                           argv[i]);
            }
        } else if (std::strcmp(argv[i], "-con") == 0) {
            ENFORCE_ARGUMENT("-con", i)
            m_outMapName = argv[i];
        } else if (std::strcmp(argv[i], "-cir") == 0) {
            m_removeInputMap = true;
        } else if (std::strcmp(argv[i], "-coc") == 0) {
            m_copyAttributes = true;
        } else if (std::strcmp(argv[i], "-crsl") == 0) {
            ENFORCE_ARGUMENT("-crsl", i)
            if (!has_only_digits_dots(argv[i])) {
                throw CommandLineException(std::string("-crsl must be a number >0, got ") +
                                           argv[i]);
            }
            m_removeStubLengthPRC = std::stod(argv[i]);
            if (!(m_removeStubLengthPRC > 0)) {
                throw CommandLineException(std::string("-crsl must be a number >0, got ") +
                                           argv[i]);
            }
        }
    }

    if (m_outMapType == ShapeMap::EMPTYMAP) {
        throw CommandLineException("A valid output map type (-co) is required");
    }

    if (m_outMapName == "") {
        throw CommandLineException("A valid output map name (-con) is required");
    }
}

void MapConvertParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    int currentMapType = metaGraph.getDisplayedMapType();

    if (currentMapType == ShapeMap::EMPTYMAP) {
        if (metaGraph.hasVisibleDrawingLayers()) {
            currentMapType = ShapeMap::DRAWINGMAP;
        } else {
            throw depthmapX::RuntimeException("No currently available map to convert from");
        }
    }

    if (copyAttributes()) {
        if (currentMapType != ShapeMap::DATAMAP && currentMapType != ShapeMap::AXIALMAP &&
            currentMapType != ShapeMap::SEGMENTMAP) {
            throw depthmapX::RuntimeException("Copying attributes is only available when "
                                              "converting between Data, Axial and Segment maps "
                                              "(current map type is not of those types)");
        }
        if (outputMapType() != ShapeMap::DATAMAP && outputMapType() != ShapeMap::AXIALMAP &&
            outputMapType() != ShapeMap::SEGMENTMAP) {
            throw depthmapX::RuntimeException("Copying attributes is only available when "
                                              "converting between Data, Axial and Segment maps "
                                              "(selected output map type is not of those types)");
        }
    }
    if (removeStubLength() > 0) {
        if (currentMapType != ShapeMap::AXIALMAP) {
            throw depthmapX::RuntimeException("Removing stubs (-crsl) is only available when"
                                              "converting from Axial to Segment maps"
                                              "(current map type is not Axial)");
        }
        if (outputMapType() != ShapeMap::SEGMENTMAP) {
            throw depthmapX::RuntimeException("Removing stubs (-crsl) is only available when"
                                              "converting from Axial to Segment maps"
                                              "(selected output map type is not Segment)");
        }
    }

    switch (outputMapType()) {
    case ShapeMap::DRAWINGMAP: {
        DO_TIMED("Converting to drawing",
                 metaGraph.convertToDrawing(dm_runmethods::getCommunicator(clp).get(),
                                            outputMapName(), currentMapType == ShapeMap::DATAMAP));

        if (mimicVersion.has_value() && *mimicVersion == "depthmapX 0.8.0") {
            // this version does not actually set the map type of the space pixels
            for (auto &map : metaGraph.getDrawingFiles().back().maps) {
                map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
            }
        }
        break;
    }
    case ShapeMap::AXIALMAP: {
        switch (currentMapType) {
        case ShapeMap::DRAWINGMAP: {
            DO_TIMED("Converting from drawing to axial",
                     metaGraph.convertDrawingToAxial(dm_runmethods::getCommunicator(clp).get(),
                                                     outputMapName()));
            break;
        }
        case ShapeMap::DATAMAP: {
            DO_TIMED("Converting from data to axial",
                     metaGraph.convertDataToAxial(dm_runmethods::getCommunicator(clp).get(),
                                                  outputMapName(), !removeInputMap(),
                                                  copyAttributes()));
            break;
        }
        default: {
            throw depthmapX::RuntimeException("Unsupported conversion to axial");
        }
        }
        if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = metaGraph.getShapeGraphs().back();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }
        break;
    }
    case ShapeMap::SEGMENTMAP: {
        switch (currentMapType) {
        case ShapeMap::DRAWINGMAP: {
            DO_TIMED("Converting from drawing to segment",
                     metaGraph.convertDrawingToSegment(dm_runmethods::getCommunicator(clp).get(),
                                                       outputMapName()));
            break;
        }
        case ShapeMap::AXIALMAP: {
            DO_TIMED("Converting from axial to segment",
                     metaGraph.convertAxialToSegment(dm_runmethods::getCommunicator(clp).get(),
                                                     outputMapName(), !removeInputMap(),
                                                     copyAttributes(), removeStubLength() / 100.0));
            break;
        }
        case ShapeMap::DATAMAP: {
            DO_TIMED("Converting from data to segment",
                     metaGraph.convertDataToSegment(dm_runmethods::getCommunicator(clp).get(),
                                                    outputMapName(), !removeInputMap(),
                                                    copyAttributes()));
            break;
        }
        default: {
            throw depthmapX::RuntimeException("Unsupported conversion to segment");
        }
        }
        if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = metaGraph.getShapeGraphs().back();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }
        break;
    }
    case ShapeMap::DATAMAP: {
        DO_TIMED("Converting to data",
                 metaGraph.convertToData(dm_runmethods::getCommunicator(clp).get(), outputMapName(),
                                         !removeInputMap(), currentMapType, copyAttributes()));
        if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = metaGraph.getDataMaps().back();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }
        break;
    }
    case ShapeMap::CONVEXMAP: {
        DO_TIMED("Converting to convex",
                 metaGraph.convertToConvex(dm_runmethods::getCommunicator(clp).get(),
                                           outputMapName(), !removeInputMap(), currentMapType,
                                           copyAttributes()));
        if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = metaGraph.getShapeGraphs().back();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }
        break;
    }
    default: {
        throw depthmapX::RuntimeException("Unsupported conversion");
    }
    }

    std::cout << " ok\nWriting out result..." << std::flush;
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
