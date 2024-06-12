// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commandlineparser.h"
#include "modeparserregistry.h"
#include "performancewriter.h"

#include <iostream>

int main(int argc, char *argv[]) {
    ModeParserRegistry registry;
    CommandLineParser args(registry);
    try {
        args.parse(static_cast<size_t>(argc), argv);
        if (!args.isValid()) {
            if (args.printVersionMode()) {
                args.printVersion();
            } else {
                args.printHelp();
            }
            return 0;
        }

        PerformanceWriter perfWriter(args.getTimingFile());

        args.run(perfWriter);
        perfWriter.write();

    } catch (std::exception &e) {
        std::cout << e.what() << "\n"
                  << "Type 'depthmapXcli -h' for help" << std::endl;
        return -1;
    }
    return 0;
}
