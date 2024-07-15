// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "runmethods.h"

#include "printcommunicator.h"
#include "simpletimer.h"

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
    void writeGraph(const CommandLineParser &clp, MetaGraphDX &metaGraph,
                    const std::string &filename, bool currentlayer) {
        metaGraph.write(filename, METAGRAPH_VERSION, currentlayer, clp.ignoreDisplayData());
    }
} // namespace dm_runmethods
