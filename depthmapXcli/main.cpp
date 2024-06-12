// Copyright (C) 2017, Christian Sailer

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
