// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "runmethods.h"

#include "exceptions.h"
#include "printcommunicator.h"
#include "radiusconverter.h"
#include "salalib/agents/agentanalysis.h"
#include "simpletimer.h"

#include "salalib/entityparsing.h"
#include "salalib/exportutils.h"
#include "salalib/gridproperties.h"
#include "salalib/importutils.h"
#include "salalib/linkutils.h"

#include <memory>
#include <sstream>
#include <vector>

namespace dm_runmethods {
    MetaGraphDX loadGraph(const std::string &filename, IPerformanceSink &perfWriter) {
        std::cout << "Loading graph " << filename << std::flush;
        MetaGraphDX mgraph("Test mgraph");
        DO_TIMED("Load graph file", mgraph.readFromFile(filename);)

        if (mgraph.getReadStatus() != MetaGraphReadWrite::ReadStatus::OK) {
            std::stringstream message;
            message << "Failed to load graph from file " << filename << ", error "
                    << MetaGraphReadWrite::getReadMessage(mgraph.getReadStatus()) << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
        }
        std::cout << " ok\n" << std::flush;
        return mgraph;
    }

    std::unique_ptr<Communicator> getCommunicator(const CommandLineParser &clp) {
        if (clp.printProgress()) {
            return std::unique_ptr<Communicator>(new PrintCommunicator());
        }
        return nullptr;
    }

    void importFiles(const CommandLineParser &cmdP, const ImportParser &parser,
                     IPerformanceSink &perfWriter) {
        std::ifstream mainFileStream(cmdP.getFileName().c_str());
        if (!mainFileStream.good()) {
            std::stringstream message;
            message << "File not found: " << cmdP.getFileName() << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
        }

        MetaGraphDX mGraph("Test mgraph");
        DO_TIMED("Load graph file", mGraph.readFromFile(cmdP.getFileName());)

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        if (mGraph.getReadStatus() == MetaGraphReadWrite::ReadStatus::NOT_A_GRAPH) {
            // not a graph, try to import the file
            std::string ext = cmdP.getFileName().substr(cmdP.getFileName().length() - 4,
                                                        cmdP.getFileName().length() - 1);
            std::ifstream file(cmdP.getFileName());

            bool asDrawingLines = false;
            depthmapX::ImportFileType importFileType = depthmapX::ImportFileType::TSV;
            if (dXstring::toLower(ext) == ".csv") {
                importFileType = depthmapX::ImportFileType::CSV;
            } else if (dXstring::toLower(ext) == ".dxf") {
                importFileType = depthmapX::ImportFileType::DXF;
                asDrawingLines = true;
            }
            if (asDrawingLines) {
                auto newDrawingFile = mGraph.loadLineData(
                    getCommunicator(cmdP).get(), cmdP.getFileName(), importFileType, false);
                if (mimickVersion.has_value() && *mimickVersion == "depthmapX 0.8.0") {
                    // this version does not actually set the map type of the space pixels
                    for (auto &map : mGraph.getDrawingFiles()[newDrawingFile].maps) {
                        map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
                    }
                }
            } else {
                auto newMaps =
                    depthmapX::importFile(file, getCommunicator(cmdP).get(), cmdP.getFileName(),
                                          parser.getImportMapType(), importFileType);
                if (parser.getImportMapType() == depthmapX::ImportType::DATAMAP) {
                    for (auto &&map : newMaps) {
                        mGraph.getDataMaps().emplace_back(
                            std::make_unique<ShapeMap>(std::move(map)));
                    }
                    if (!mGraph.getDataMaps().empty()) {
                        mGraph.setDisplayedDataMapRef(mGraph.getDataMaps().size() - 1);
                        mGraph.setState(mGraph.getState() | MetaGraphDX::DATAMAPS);
                        mGraph.setViewClass(MetaGraphDX::SHOWHIDESHAPE);
                    }
                } else {
                    auto newDrawingFile =
                        mGraph.addDrawingFile(cmdP.getFileName(), std::move(newMaps));
                    mGraph.setState(mGraph.getState() | MetaGraphDX::LINEDATA);
                    if (mimickVersion.has_value() && *mimickVersion == "depthmapX 0.8.0") {
                        // this version does not actually set the map type of the space pixels
                        for (auto &map : mGraph.getDrawingFiles()[newDrawingFile].maps) {
                            map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
                        }
                    }
                }
            }
        } else if (mGraph.getReadStatus() == MetaGraphReadWrite::ReadStatus::OK) {
            if (parser.toImportAsAttrbiutes()) {

                if (mGraph.getDisplayedMapType() == ShapeMap::EMPTYMAP) {
                    throw depthmapX::RuntimeException("No map displayed to attach attributes to");
                }

                std::vector<std::string> fileNames = parser.getFilesToImport();
                for (std::string &fileName : fileNames) {
                    std::string ext = fileName.substr(fileName.length() - 4, fileName.length() - 1);
                    std::ifstream file(fileName);
                    char delimiter = '\t';
                    if (dXstring::toLower(ext) == ".csv") {
                        delimiter = ',';
                    }

                    DO_TIMED("Importing attributes",
                             depthmapX::importAttributes(mGraph.getDisplayedMapAttributes(), file,
                                                         delimiter);)
                }
            }
        }
        DO_TIMED("Writing graph",
                 mGraph.write(cmdP.getOuputFile().c_str(), METAGRAPH_VERSION, false);)
    }

    void linkGraph(const CommandLineParser &cmdP, const LinkParser &parser,
                   IPerformanceSink &perfWriter) {

        auto mgraph = loadGraph(cmdP.getFileName().c_str(), perfWriter);

        if (parser.getLinkMode() == LinkParser::LinkMode::UNLINK &&
            parser.getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS &&
            mgraph.getDisplayedShapeGraph().getMapType() != ShapeMap::AXIALMAP) {
            throw depthmapX::RuntimeException(
                "Unlinking is only available for axial maps and pointmaps");
        }

        char delimiter = '\t';
        std::stringstream linksStream;
        if (!parser.getLinksFile().empty()) {
            std::ifstream fileStream(parser.getLinksFile());
            if (!linksStream) {
                std::stringstream message;
                message << "Failed to load file " << parser.getLinksFile() << ", error "
                        << std::flush;
                throw depthmapX::RuntimeException(message.str().c_str());
            }
            linksStream << fileStream.rdbuf();
            fileStream.close();
        } else if (!parser.getManualLinks().empty()) {
            delimiter = ',';
            std::string header = "x1,y1,x2,y2";
            if (parser.getLinkType() == LinkParser::LinkType::REFS) {
                header = "reffrom,refto";
            } else if (parser.getLinkMode() == LinkParser::LinkMode::UNLINK) {
                header = "x,y";
            }
            linksStream << header;
            auto iter = parser.getManualLinks().begin(), end = parser.getManualLinks().end();
            for (; iter != end; ++iter) {
                linksStream << "\n" << *iter;
            }
        }

        SimpleTimer t;
        if (parser.getLinkMode() == LinkParser::LinkMode::LINK) {
            if (parser.getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS) {
                auto &shapeGraph = mgraph.getDisplayedShapeGraph();
                if (parser.getLinkType() == LinkParser::LinkType::COORDS) {
                    std::vector<Line> mergeLines =
                        EntityParsing::parseLines(linksStream, delimiter);
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
                auto &currentMap = mgraph.getDisplayedPointMap();
                if (parser.getLinkType() == LinkParser::LinkType::COORDS) {
                    std::vector<Line> mergeLines =
                        EntityParsing::parseLines(linksStream, delimiter);
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
            if (parser.getMapTypeGroup() == LinkParser::MapTypeGroup::SHAPEGRAPHS) {
                auto &shapeGraph = mgraph.getDisplayedShapeGraph();
                if (parser.getLinkType() == LinkParser::LinkType::COORDS) {
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
                auto &currentMap = mgraph.getDisplayedPointMap();
                if (parser.getLinkType() == LinkParser::LinkType::COORDS) {
                    std::vector<Line> mergeLines =
                        EntityParsing::parseLines(linksStream, delimiter);
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
                 mgraph.write(cmdP.getOuputFile().c_str(), METAGRAPH_VERSION, false);)
    }

    void runVga(const CommandLineParser &cmdP, const VgaParser &vgaP,
                const IRadiusConverter &converter, IPerformanceSink &perfWriter) {
        auto mgraph = loadGraph(cmdP.getFileName().c_str(), perfWriter);

        std::unique_ptr<Options> options(new Options());

        std::cout << "Getting options..." << std::flush;
        switch (vgaP.getVgaMode()) {
        case VgaParser::VgaMode::VISBILITY:
            options->output_type = AnalysisType::VISUAL;
            options->local = vgaP.localMeasures();
            options->global = vgaP.globalMeasures();
            if (options->global) {
                options->radius = converter.ConvertForVisibility(vgaP.getRadius());
            }
            break;
        case VgaParser::VgaMode::METRIC:
            options->output_type = AnalysisType::METRIC;
            options->radius = converter.ConvertForMetric(vgaP.getRadius());
            break;
        case VgaParser::VgaMode::ANGULAR:
            options->output_type = AnalysisType::ANGULAR;
            break;
        case VgaParser::VgaMode::ISOVIST:
            options->output_type = AnalysisType::ISOVIST;
            break;
        case VgaParser::VgaMode::THRU_VISION:
            options->output_type = AnalysisType::THRU_VISION;
            break;
        default:
            throw depthmapX::SetupCheckException("Unsupported VGA mode");
        }
        std::cout << " ok\nAnalysing graph..." << std::flush;

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        if (!mimickVersion.has_value()) {
            // current version
            DO_TIMED("Run VGA",
                     mgraph.analyseGraph(getCommunicator(cmdP).get(), *options, cmdP.simpleMode());)

        } else if (*mimickVersion == "depthmapX 0.8.0") {
            int currentDisplayedAttribute = -1;
            if (vgaP.getVgaMode() == VgaParser::VgaMode::ISOVIST) {
                // in this version vga isovist analysis does not change the
                // displayed attribute, so we have to reset it back to what
                // it was before the analysis
                currentDisplayedAttribute = mgraph.getDisplayedPointMap().getDisplayedAttribute();
            }

            DO_TIMED("Run VGA",
                     mgraph.analyseGraph(getCommunicator(cmdP).get(), *options, cmdP.simpleMode());)

            if (vgaP.getVgaMode() == VgaParser::VgaMode::ISOVIST) {
                mgraph.getDisplayedPointMap().setDisplayedAttribute(currentDisplayedAttribute);
            }
            /* legacy mode where the columns are sorted before stored */
            for (auto &map : mgraph.getPointMaps()) {
                auto displayedAttribute = map.getDisplayedAttribute();

                auto sortedDisplayedAttribute =
                    static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                        static_cast<size_t>(displayedAttribute)));
                map.setDisplayedAttribute(sortedDisplayedAttribute);
            }
        }

        std::cout << " ok\nWriting out result..." << std::flush;
        DO_TIMED("Writing graph",
                 mgraph.write(cmdP.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void fillGraph(MetaGraphDX &graph, const Point2f &point) {
        auto r = graph.getRegion();
        if (!r.contains(point)) {
            throw depthmapX::RuntimeException("Point outside of target region");
        }
        graph.makePoints(point, 0, nullptr);
    }

    void runVisualPrep(const CommandLineParser &clp, double gridSize,
                       const std::vector<Point2f> &fillPoints, double maxVisibility,
                       bool boundaryGraph, bool makeGraph, bool unmakeGraph,
                       bool removeLinksWhenUnmaking, IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        std::cout << "Initial checks... " << std::flush;
        auto state = mGraph.getState();
        if (~state & MetaGraphDX::LINEDATA) {
            throw depthmapX::RuntimeException("Graph must have line data before preparing VGA");
        }
        if (gridSize > 0) {
            // Create a new pointmap and set tha grid
            QtRegion r = mGraph.getRegion();

            GridProperties gp(__max(r.width(), r.height()));
            if (gridSize > gp.getMax() || gridSize < gp.getMin()) {
                std::stringstream message;
                message << "Chosen grid spacing " << gridSize
                        << " is outside of the expected interval of " << gp.getMin()
                        << " <= spacing <= " << gp.getMax() << std::flush;
                throw depthmapX::RuntimeException(message.str());
            }

            std::cout << "ok\nSetting up grid... " << std::flush;
            mGraph.addNewPointMap();
            DO_TIMED("Setting grid", mGraph.setGrid(gridSize, Point2f(0.0, 0.0)))
        } else if (mGraph.getPointMaps().empty()) {
            std::stringstream message;
            message << "No map exists to use. Please create a new one by providing a grid size"
                    << std::flush;
            throw depthmapX::RuntimeException(message.str());
        }

        if (unmakeGraph) {
            if (!mGraph.getDisplayedPointMap().getInternalMap().isProcessed()) {
                std::stringstream message;
                message << "Current map has not had its graph made so there's nothing to unmake"
                        << std::flush;
                throw depthmapX::RuntimeException(message.str());
            }
            DO_TIMED("Unmaking graph", mGraph.unmakeGraph(removeLinksWhenUnmaking))
        } else {
            if (fillPoints.size() > 0) {
                std::cout << "ok\nFilling grid... " << std::flush;
                DO_TIMED("Filling grid", for_each(fillPoints.begin(), fillPoints.end(),
                                                  [&mGraph](const Point2f &point) -> void {
                                                      fillGraph(mGraph, point);
                                                  }))
            }
            if (makeGraph) {
                std::cout << "ok\nMaking graph... " << std::flush;
                DO_TIMED("Making graph", mGraph.makeGraph(getCommunicator(clp).get(),
                                                          boundaryGraph ? 1 : 0, maxVisibility))

                if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                    /* legacy mode where the columns are sorted before stored */
                    auto &map = mGraph.getDisplayedPointMap();
                    auto displayedAttribute = map.getDisplayedAttribute();

                    auto sortedDisplayedAttribute =
                        static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                            static_cast<size_t>(displayedAttribute)));
                    map.setDisplayedAttribute(sortedDisplayedAttribute);
                }
            }
        }

        std::cout << " ok\nWriting out result..." << std::flush;
        DO_TIMED("Writing graph",
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void runAxialAnalysis(const CommandLineParser &clp, const AxialParser &ap,
                          IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        auto state = mGraph.getState();
        if (ap.runAllLines()) {
            if (~state & MetaGraphDX::LINEDATA) {
                throw depthmapX::RuntimeException(
                    "Line drawing must be loaded before axial map can be constructed");
            }
            std::cout << "Making all line map... " << std::flush;
            DO_TIMED("Making all axes map",
                     for_each(ap.getAllAxesRoots().begin(), ap.getAllAxesRoots().end(),
                              [&mGraph, &clp](const Point2f &point) -> void {
                                  mGraph.makeAllLineMap(getCommunicator(clp).get(), point);
                              }))
            std::cout << "ok" << std::endl;
        }

        if (ap.runFewestLines()) {
            if (~state & MetaGraphDX::LINEDATA) {
                throw depthmapX::RuntimeException(
                    "Line drawing must be loaded before fewest line map can be constructed");
            }
            if (!mGraph.hasAllLineMap()) {
                throw depthmapX::RuntimeException("All line map must be constructed before fewest "
                                                  "lines can be constructed. Use -aa to do this");
            }
            std::cout << "Constructing fewest line map... " << std::flush;
            DO_TIMED("Fewest line map", mGraph.makeFewestLineMap(getCommunicator(clp).get(), 1))
            std::cout << "ok" << std::endl;
        }

        if (ap.runAnalysis()) {
            std::cout << "Running axial analysis... " << std::flush;
            Options options;
            const std::vector<double> &radii = ap.getRadii();
            options.radius_set.insert(radii.begin(), radii.end());
            options.choice = ap.useChoice();
            options.local = ap.useLocal();
            options.fulloutput = ap.calculateRRA();
            options.weighted_measure_col = -1;

            if (!ap.getAttribute().empty()) {
                const auto &map = mGraph.getDisplayedShapeGraph();
                const auto &table = map.getAttributeTable();
                for (size_t i = 0; i < table.getNumColumns(); i++) {
                    if (ap.getAttribute() == table.getColumnName(i).c_str()) {
                        options.weighted_measure_col = static_cast<int>(i);
                    }
                }
                if (options.weighted_measure_col == -1) {
                    throw depthmapX::RuntimeException("Given attribute (" + ap.getAttribute() +
                                                      ") does not exist in currently selected map");
                }
            }

            DO_TIMED("Axial analysis", mGraph.analyseAxial(getCommunicator(clp).get(), options,
                                                           (mimickVersion.has_value() &&
                                                            mimickVersion == "depthmapX 0.8.0")))
            std::cout << "ok\n" << std::flush;
        }

        if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = mGraph.getDisplayedShapeGraph();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }

        std::cout << "Writing out result..." << std::flush;
        DO_TIMED("Writing graph",
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void runSegmentAnalysis(const CommandLineParser &clp, const SegmentParser &sp,
                            IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        std::cout << "Running segment analysis... " << std::flush;
        Options options;
        const std::vector<double> &radii = sp.getRadii();
        options.radius_set.insert(radii.begin(), radii.end());
        options.choice = sp.includeChoice();
        options.tulip_bins = sp.getTulipBins();
        options.weighted_measure_col = -1;

        if (!sp.getAttribute().empty()) {
            const auto &map = mGraph.getDisplayedShapeGraph();
            const AttributeTable &table = map.getAttributeTable();
            for (size_t i = 0; i < table.getNumColumns(); i++) {
                if (sp.getAttribute() == table.getColumnName(i).c_str()) {
                    options.weighted_measure_col = static_cast<int>(i);
                }
            }
            if (options.weighted_measure_col == -1) {
                throw depthmapX::RuntimeException("Given attribute (" + sp.getAttribute() +
                                                  ") does not exist in currently selected map");
            }
        }

        switch (sp.getRadiusType()) {
        case SegmentParser::RadiusType::SEGMENT_STEPS: {
            options.radius_type = RadiusType::TOPOLOGICAL;
            break;
        }
        case SegmentParser::RadiusType::METRIC: {
            options.radius_type = RadiusType::METRIC;
            break;
        }
        case SegmentParser::RadiusType::ANGULAR: {
            options.radius_type = RadiusType::ANGULAR;
            break;
        }
        case SegmentParser::RadiusType::NONE:
            break;
        }
        switch (sp.getAnalysisType()) {
        case SegmentParser::AnalysisType::ANGULAR_TULIP: {
            DO_TIMED("Segment tulip analysis",
                     mGraph.analyseSegmentsTulip(
                         getCommunicator(clp).get(), options,
                         (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0")))
            break;
        }
        case SegmentParser::AnalysisType::ANGULAR_FULL: {
            DO_TIMED("Segment angular analysis",
                     mGraph.analyseSegmentsAngular(getCommunicator(clp).get(), options))
            break;
        }
        case SegmentParser::AnalysisType::TOPOLOGICAL: {
            options.output_type = AnalysisType::ISOVIST;
            DO_TIMED("Segment topological",
                     mGraph.analyseTopoMetMultipleRadii(getCommunicator(clp).get(), options))
            break;
        }
        case SegmentParser::AnalysisType::METRIC: {
            options.output_type = AnalysisType::VISUAL;
            DO_TIMED("Segment metric",
                     mGraph.analyseTopoMetMultipleRadii(getCommunicator(clp).get(), options))
            break;
        }
        case SegmentParser::AnalysisType::NONE:
            throw depthmapX::RuntimeException("No segment analysis type given");
        }
        std::cout << "ok\n" << std::flush;

        if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = mGraph.getDisplayedShapeGraph();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }

        std::cout << "Writing out result..." << std::flush;
        DO_TIMED("Writing graph",
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void runAgentAnalysis(const CommandLineParser &cmdP, const AgentParser &agentP,
                          IPerformanceSink &perfWriter) {

        auto mGraph = loadGraph(cmdP.getFileName().c_str(), perfWriter);

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        auto &currentMap = mGraph.getDisplayedPointMap();

        int agentViewAlgorithm = AgentProgram::SEL_STANDARD;

        switch (agentP.getAgentMode()) {
        case AgentParser::NONE:
        case AgentParser::STANDARD:
            agentViewAlgorithm = AgentProgram::SEL_STANDARD;
            break;
        case AgentParser::LOS_LENGTH:
            agentViewAlgorithm = AgentProgram::SEL_LOS;
            break;
        case AgentParser::OCC_LENGTH:
            agentViewAlgorithm = AgentProgram::SEL_LOS_OCC;
            break;
        case AgentParser::OCC_ANY:
            agentViewAlgorithm = AgentProgram::SEL_OCC_ALL;
            break;
        case AgentParser::OCC_GROUP_45:
            agentViewAlgorithm = AgentProgram::SEL_OCC_BIN45;
            break;
        case AgentParser::OCC_GROUP_60:
            agentViewAlgorithm = AgentProgram::SEL_OCC_BIN60;
            break;
        case AgentParser::OCC_FURTHEST:
            agentViewAlgorithm = AgentProgram::SEL_OCC_STANDARD;
            break;
        case AgentParser::BIN_FAR_DIST:
            agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_DIST;
            break;
        case AgentParser::BIN_ANGLE:
            agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_ANG;
            break;
        case AgentParser::BIN_FAR_DIST_ANGLE:
            agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_DIST_ANG;
            break;
        case AgentParser::BIN_MEMORY:
            agentViewAlgorithm = AgentProgram::SEL_OCC_MEMORY;
            break;
        }

        std::optional<AgentAnalysis::TrailRecordOptions> recordTrails =
            agentP.recordTrailsForAgents() >= 0
                ? std::make_optional(AgentAnalysis::TrailRecordOptions{
                      agentP.recordTrailsForAgents() == 0
                          ? std::nullopt
                          : std::make_optional(static_cast<size_t>(agentP.recordTrailsForAgents())),
                      std::ref(mGraph.getDataMaps()
                                   .emplace_back("Agent Trails", ShapeMap::DATAMAP)
                                   .getInternalMap())})
                : std::nullopt;

        if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
            // older versions of depthmapX limited the maximum number of trails to 50
            if (recordTrails.has_value()) {
                if ((recordTrails->limit.has_value() && recordTrails->limit > 50) ||
                    !recordTrails->limit.has_value()) {
                    recordTrails->limit = 50;
                }
            }
        }

        // the ui and code suggest that the results can be put on a separate
        // 'data map', but the functionality does not seem to actually be
        // there thus it is skipped for now
        std::optional<std::reference_wrapper<ShapeMap>> gateLayer = std::nullopt;

        auto analysis = std::unique_ptr<IAnalysis>(new AgentAnalysis(
            currentMap.getInternalMap(), agentP.totalSystemTimestemps(), agentP.releaseRate(),
            static_cast<size_t>(agentP.agentLifeTimesteps()),
            static_cast<unsigned short>(agentP.agentFOV()),
            static_cast<size_t>(agentP.agentStepsBeforeTurnDecision()), agentViewAlgorithm,
            agentP.randomReleaseLocationSeed(), agentP.getReleasePoints(), gateLayer,
            recordTrails));

        std::cout << "ok\nRunning agent analysis... " << std::flush;
        DO_TIMED("Running agent analysis",
                 mGraph.runAgentEngine(getCommunicator(cmdP).get(), analysis);)

        std::cout << " ok\nWriting out result..." << std::flush;
        std::vector<AgentParser::OutputType> resultTypes = agentP.outputTypes();
        if (resultTypes.size() == 0) {
            // if no choice was made for an output type assume the user just
            // wants a graph file

            std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

            if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = mGraph.getDisplayedPointMap();
                auto displayedAttribute = map.getDisplayedAttribute();

                auto sortedDisplayedAttribute =
                    static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                        static_cast<size_t>(displayedAttribute)));
                map.setDisplayedAttribute(sortedDisplayedAttribute);
            }

            DO_TIMED("Writing graph",
                     mGraph.write(cmdP.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        } else if (resultTypes.size() == 1) {
            // if only one type of output is given, assume that the user has
            // correctly entered a name with the correct extension and export
            // exactly with that name and extension

            switch (resultTypes[0]) {
            case AgentParser::OutputType::GRAPH: {

                std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

                if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                    /* legacy mode where the columns are sorted before stored */
                    auto &map = mGraph.getDisplayedPointMap();
                    auto displayedAttribute = map.getDisplayedAttribute();

                    auto sortedDisplayedAttribute =
                        static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                            static_cast<size_t>(displayedAttribute)));
                    map.setDisplayedAttribute(sortedDisplayedAttribute);
                }

                DO_TIMED("Writing graph",
                         mGraph.write(cmdP.getOuputFile().c_str(), METAGRAPH_VERSION, false))
                break;
            }
            case AgentParser::OutputType::GATECOUNTS: {
                std::ofstream gatecountStream(cmdP.getOuputFile().c_str());
                DO_TIMED("Writing gatecounts",
                         currentMap.getInternalMap().outputSummary(gatecountStream, ','))
                break;
            }
            case AgentParser::OutputType::TRAILS: {
                std::ofstream trailStream(cmdP.getOuputFile().c_str());
                DO_TIMED("Writing trails",
                         exportUtils::writeMapShapesAsCat(recordTrails->map, trailStream))

                break;
            }
            }
        } else {
            // if more than one output type is given assume the user has given
            // a filename without an extension and thus the new file must have
            // an extension. Also to avoid name clashes in cases where the user
            // asked for outputs that would yield the same extension also add
            // a related suffix

            if (std::find(resultTypes.begin(), resultTypes.end(), AgentParser::OutputType::GRAPH) !=
                resultTypes.end()) {

                std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

                if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                    /* legacy mode where the columns are sorted before stored */
                    for (auto &map : mGraph.getShapeGraphs()) {
                        auto displayedAttribute = map.getDisplayedAttribute();

                        auto sortedDisplayedAttribute =
                            static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                                static_cast<size_t>(displayedAttribute)));
                        map.setDisplayedAttribute(sortedDisplayedAttribute);
                    }
                    auto &map = mGraph.getDisplayedPointMap();
                    auto displayedAttribute = map.getDisplayedAttribute();

                    auto sortedDisplayedAttribute =
                        static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                            static_cast<size_t>(displayedAttribute)));
                    map.setDisplayedAttribute(sortedDisplayedAttribute);
                }

                std::string outFile = cmdP.getOuputFile() + ".graph";
                DO_TIMED("Writing graph", mGraph.write(outFile.c_str(), METAGRAPH_VERSION, false))
            }
            if (std::find(resultTypes.begin(), resultTypes.end(),
                          AgentParser::OutputType::GATECOUNTS) != resultTypes.end()) {
                std::string outFile = cmdP.getOuputFile() + "_gatecounts.csv";
                std::ofstream gatecountStream(outFile.c_str());
                DO_TIMED("Writing gatecounts",
                         currentMap.getInternalMap().outputSummary(gatecountStream, ','))
            }
            if (std::find(resultTypes.begin(), resultTypes.end(),
                          AgentParser::OutputType::TRAILS) != resultTypes.end()) {
                std::string outFile = cmdP.getOuputFile() + "_trails.cat";
                std::ofstream trailStream(outFile.c_str());
                DO_TIMED("Writing trails",
                         exportUtils::writeMapShapesAsCat(recordTrails->map, trailStream))
            }
        }
    }

    void runIsovists(const CommandLineParser &clp, const std::vector<IsovistDefinition> &isovists,
                     IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::cout << "Making " << isovists.size() << " isovists... " << std::flush;
        DO_TIMED("Make isovists",
                 std::for_each(isovists.begin(), isovists.end(),
                               [&mGraph, &clp](const IsovistDefinition &isovist) -> void {
                                   mGraph.makeIsovist(getCommunicator(clp).get(),
                                                      isovist.getLocation(), isovist.getLeftAngle(),
                                                      isovist.getRightAngle(), clp.simpleMode());
                               }))
        std::cout << " ok\nWriting out result..." << std::flush;

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */

            auto &map = mGraph.getDataMaps().back();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }

        DO_TIMED("Writing graph",
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void exportData(const CommandLineParser &cmdP, const ExportParser &exportP,
                    IPerformanceSink &perfWriter) {

        auto mgraph = loadGraph(cmdP.getFileName().c_str(), perfWriter);

        switch (exportP.getExportMode()) {
        case ExportParser::POINTMAP_DATA_CSV: {
            auto &currentMap = mgraph.getDisplayedPointMap();
            std::ofstream stream(cmdP.getOuputFile().c_str());
            DO_TIMED("Writing pointmap data",
                     currentMap.getInternalMap().outputSummary(stream, ','))
            stream.close();
            break;
        }
        case ExportParser::POINTMAP_CONNECTIONS_CSV: {
            auto &currentMap = mgraph.getDisplayedPointMap();
            std::ofstream stream(cmdP.getOuputFile().c_str());
            DO_TIMED("Writing pointmap connections",
                     currentMap.getInternalMap().outputConnectionsAsCSV(stream, ","))
            stream.close();
            break;
        }
        case ExportParser::POINTMAP_LINKS_CSV: {
            auto &currentMap = mgraph.getDisplayedPointMap();
            std::ofstream stream(cmdP.getOuputFile().c_str());
            DO_TIMED("Writing pointmap connections",
                     currentMap.getInternalMap().outputLinksAsCSV(stream, ","))
            stream.close();
            break;
        }
        case ExportParser::SHAPEGRAPH_MAP_CSV: {
            auto &currentMap = mgraph.getDisplayedShapeGraph();
            std::ofstream stream(cmdP.getOuputFile().c_str());
            DO_TIMED("Writing pointmap connections",
                     currentMap.getInternalMap().output(stream, ','))
            stream.close();
            break;
        }
        case ExportParser::SHAPEGRAPH_MAP_MIF: {
            auto &currentMap = mgraph.getDisplayedShapeGraph();
            std::string fileName = cmdP.getOuputFile().c_str();
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
            std::ofstream stream(cmdP.getOuputFile().c_str());
            DO_TIMED("Writing shapegraph connections",
                     currentMap.getInternalMap().isAxialMap()
                         ? currentMap.getInternalMap().writeAxialConnectionsAsPairsCSV(stream)
                         : currentMap.getInternalMap().writeSegmentConnectionsAsPairsCSV(stream))
            stream.close();
            break;
        }
        case ExportParser::SHAPEGRAPH_LINKS_UNLINKS_CSV: {
            auto &currentMap = mgraph.getDisplayedShapeGraph();
            std::ofstream stream(cmdP.getOuputFile().c_str());
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

    void runStepDepth(const CommandLineParser &clp, const StepDepthParser::StepType &stepType,
                      const std::vector<Point2f> &stepDepthPoints, IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::cout << "ok\nSelecting cells... " << std::flush;

        for (auto &point : stepDepthPoints) {
            auto graphRegion = mGraph.getRegion();
            if (!graphRegion.contains(point)) {
                throw depthmapX::RuntimeException("Point outside of target region");
            }
            QtRegion r(point, point);
            mGraph.setCurSel(r, true);
        }

        std::cout << "ok\nCalculating step-depth... " << std::flush;

        Options options;
        options.global = 0;

        switch (stepType) {
        case StepDepthParser::StepType::ANGULAR:
            options.point_depth_selection = 3;
            break;
        case StepDepthParser::StepType::METRIC:
            options.point_depth_selection = 2;
            break;
        case StepDepthParser::StepType::VISUAL:
            options.point_depth_selection = 1;
            break;
        default: {
            throw depthmapX::SetupCheckException("Error, unsupported step type");
        }
        }

        DO_TIMED("Calculating step-depth",
                 mGraph.analyseGraph(getCommunicator(clp).get(), options, false);)

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */

            auto &map = mGraph.getDisplayedPointMap();
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);

            // sala no longer stores points as "selected", but previous
            // versions do. Fake-select the origin point
            int selState = 0x0010;
            auto &selSet = map.getSelSet();
            for (auto &sel : selSet) {
                auto &point = map.getPoint(sel);
                point.set(point.getState() | selState);
            }
        }

        std::cout << " ok\nWriting out result..." << std::flush;
        DO_TIMED("Writing graph",
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }

    void runMapConversion(const CommandLineParser &clp, const MapConvertParser &mcp,
                          IPerformanceSink &perfWriter) {
        auto mGraph = loadGraph(clp.getFileName().c_str(), perfWriter);

        std::optional<std::string> mimickVersion = "depthmapX 0.8.0";

        int currentMapType = mGraph.getDisplayedMapType();

        if (currentMapType == ShapeMap::EMPTYMAP) {
            if (mGraph.hasVisibleDrawingLayers()) {
                currentMapType = ShapeMap::DRAWINGMAP;
            } else {
                throw depthmapX::RuntimeException("No currently available map to convert from");
            }
        }

        if (mcp.copyAttributes()) {
            if (currentMapType != ShapeMap::DATAMAP && currentMapType != ShapeMap::AXIALMAP &&
                currentMapType != ShapeMap::SEGMENTMAP) {
                throw depthmapX::RuntimeException("Copying attributes is only available when "
                                                  "converting between Data, Axial and Segment maps "
                                                  "(current map type is not of those types)");
            }
            if (mcp.outputMapType() != ShapeMap::DATAMAP &&
                mcp.outputMapType() != ShapeMap::AXIALMAP &&
                mcp.outputMapType() != ShapeMap::SEGMENTMAP) {
                throw depthmapX::RuntimeException(
                    "Copying attributes is only available when "
                    "converting between Data, Axial and Segment maps "
                    "(selected output map type is not of those types)");
            }
        }
        if (mcp.removeStubLength() > 0) {
            if (currentMapType != ShapeMap::AXIALMAP) {
                throw depthmapX::RuntimeException("Removing stubs (-crsl) is only available when"
                                                  "converting from Axial to Segment maps"
                                                  "(current map type is not Axial)");
            }
            if (mcp.outputMapType() != ShapeMap::SEGMENTMAP) {
                throw depthmapX::RuntimeException("Removing stubs (-crsl) is only available when"
                                                  "converting from Axial to Segment maps"
                                                  "(selected output map type is not Segment)");
            }
        }

        switch (mcp.outputMapType()) {
        case ShapeMap::DRAWINGMAP: {
            DO_TIMED("Converting to drawing",
                     mGraph.convertToDrawing(getCommunicator(clp).get(), mcp.outputMapName(),
                                             currentMapType == ShapeMap::DATAMAP));

            if (mimickVersion.has_value() && *mimickVersion == "depthmapX 0.8.0") {
                // this version does not actually set the map type of the space pixels
                for (auto &map : mGraph.getDrawingFiles().back().maps) {
                    map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
                }
            }
            break;
        }
        case ShapeMap::AXIALMAP: {
            switch (currentMapType) {
            case ShapeMap::DRAWINGMAP: {
                DO_TIMED(
                    "Converting from drawing to axial",
                    mGraph.convertDrawingToAxial(getCommunicator(clp).get(), mcp.outputMapName()));
                break;
            }
            case ShapeMap::DATAMAP: {
                DO_TIMED("Converting from data to axial",
                         mGraph.convertDataToAxial(getCommunicator(clp).get(), mcp.outputMapName(),
                                                   !mcp.removeInputMap(), mcp.copyAttributes()));
                break;
            }
            default: {
                throw depthmapX::RuntimeException("Unsupported conversion to axial");
            }
            }
            if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = mGraph.getShapeGraphs().back();
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
                         mGraph.convertDrawingToSegment(getCommunicator(clp).get(),
                                                        mcp.outputMapName()));
                break;
            }
            case ShapeMap::AXIALMAP: {
                DO_TIMED("Converting from axial to segment",
                         mGraph.convertAxialToSegment(
                             getCommunicator(clp).get(), mcp.outputMapName(), !mcp.removeInputMap(),
                             mcp.copyAttributes(), mcp.removeStubLength() / 100.0));
                break;
            }
            case ShapeMap::DATAMAP: {
                DO_TIMED("Converting from data to segment",
                         mGraph.convertDataToSegment(getCommunicator(clp).get(),
                                                     mcp.outputMapName(), !mcp.removeInputMap(),
                                                     mcp.copyAttributes()));
                break;
            }
            default: {
                throw depthmapX::RuntimeException("Unsupported conversion to segment");
            }
            }
            if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = mGraph.getShapeGraphs().back();
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
                     mGraph.convertToData(getCommunicator(clp).get(), mcp.outputMapName(),
                                          !mcp.removeInputMap(), currentMapType,
                                          mcp.copyAttributes()));
            if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = mGraph.getDataMaps().back();
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
                     mGraph.convertToConvex(getCommunicator(clp).get(), mcp.outputMapName(),
                                            !mcp.removeInputMap(), currentMapType,
                                            mcp.copyAttributes()));
            if (mimickVersion.has_value() && mimickVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = mGraph.getShapeGraphs().back();
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
                 mGraph.write(clp.getOuputFile().c_str(), METAGRAPH_VERSION, false))
        std::cout << " ok" << std::endl;
    }
} // namespace dm_runmethods
