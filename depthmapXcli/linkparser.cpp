// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "linkparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

#include "salalib/entityparsing.h"
#include "salalib/linkutils.h"

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
    auto mGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    if (getLinkMode() == LinkParser::LinkMode::UNLINK &&
        getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS &&
        mGraph.getDisplayedShapeGraph().getMapType() != ShapeMap::AXIALMAP) {
        throw depthmapX::RuntimeException(
            "Unlinking is only available for axial maps and pointmaps");
    }

    char delimiter = '\t';
    std::stringstream linksStream;
    if (!getLinksFile().empty()) {
        std::ifstream fileStream(getLinksFile());
        if (!linksStream) {
            std::stringstream message;
            message << "Failed to load file " << getLinksFile() << ", error " << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
        }
        linksStream << fileStream.rdbuf();
        fileStream.close();
    } else if (!getManualLinks().empty()) {
        delimiter = ',';
        std::string header = "x1,y1,x2,y2";
        if (getLinkType() == LinkParser::LinkType::REFS) {
            header = "reffrom,refto";
        } else if (getLinkMode() == LinkParser::LinkMode::UNLINK) {
            header = "x,y";
        }
        linksStream << header;
        auto iter = getManualLinks().begin(), end = getManualLinks().end();
        for (; iter != end; ++iter) {
            linksStream << "\n" << *iter;
        }
    }

    SimpleTimer t;
    if (getLinkMode() == LinkParser::LinkMode::LINK) {
        if (getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS) {
            auto &shapeGraph = mGraph.getDisplayedShapeGraph();
            if (getLinkType() == LinkParser::LinkType::COORDS) {
                std::vector<Line> mergeLines = EntityParsing::parseLines(linksStream, delimiter);
                for (const auto &line : mergeLines) {
                    QtRegion region(line.start(), line.start());
                    shapeGraph.setCurSel(region);
                    shapeGraph.linkShapes(line.end());
                }
            } else {
                auto mergePairs = EntityParsing::parseRefPairs(linksStream, delimiter);
                for (auto pair : mergePairs) {
                    // apparently this also unlinks if already linked or crossing
                    shapeGraph.linkShapesFromRefs(pair.first, pair.second);
                }
            }
        } else {
            std::vector<PixelRefPair> newLinks;
            auto &currentMap = mGraph.getDisplayedPointMap();
            if (getLinkType() == LinkParser::LinkType::COORDS) {
                std::vector<Line> mergeLines = EntityParsing::parseLines(linksStream, delimiter);
                std::vector<PixelRefPair> linkPairsFromCoords =
                    depthmapX::pixelateMergeLines(mergeLines, currentMap.getInternalMap());
                newLinks.insert(newLinks.end(), linkPairsFromCoords.begin(),
                                linkPairsFromCoords.end());
            } else {
                auto mergePairs = EntityParsing::parseRefPairs(linksStream, delimiter);
                for (auto pair : mergePairs) {
                    newLinks.push_back(PixelRefPair(pair.first, pair.second));
                }
            }
            depthmapX::mergePixelPairs(newLinks, currentMap.getInternalMap());
        }
    } else {
        if (getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS) {
            auto &shapeGraph = mGraph.getDisplayedShapeGraph();
            if (getLinkType() == LinkParser::LinkType::COORDS) {
                auto mergePoints = EntityParsing::parsePoints(linksStream, delimiter);
                for (auto point : mergePoints) {
                    shapeGraph.getInternalMap().unlinkAtPoint(point);
                }
            } else {
                auto mergePairs = EntityParsing::parseRefPairs(linksStream, delimiter);
                for (auto pair : mergePairs) {
                    shapeGraph.unlinkShapesFromRefs(pair.first, pair.second);
                }
            }
        } else {
            std::vector<PixelRefPair> newLinks;
            auto &currentMap = mGraph.getDisplayedPointMap();
            if (getLinkType() == LinkParser::LinkType::COORDS) {
                std::vector<Line> mergeLines = EntityParsing::parseLines(linksStream, delimiter);
                std::vector<PixelRefPair> linkPairsFromCoords =
                    depthmapX::pixelateMergeLines(mergeLines, currentMap.getInternalMap());
                newLinks.insert(newLinks.end(), linkPairsFromCoords.begin(),
                                linkPairsFromCoords.end());
            } else {
                auto mergePairs = EntityParsing::parseRefPairs(linksStream, delimiter);
                for (auto pair : mergePairs) {
                    newLinks.push_back(PixelRefPair(pair.first, pair.second));
                }
            }
            depthmapX::unmergePixelPairs(newLinks, currentMap.getInternalMap());
        }
    }

    perfWriter.addData("Linking graph", t.getTimeInSeconds());
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, mGraph, clp.getOuputFile().c_str(), false))
}
