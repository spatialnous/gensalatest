// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commandlineparser.h"

#include "exceptions.h"
#include "imodeparserfactory.h"
#include "interfaceversion.h"
#include "parsingutils.h"

#include <algorithm>
#include <cstring>
#include <iostream>

using namespace depthmapX;

void CommandLineParser::printHelp() {
    std::cout << "Usage: depthmapXcli -m <mode> -f <filename> -o <output file> [-s] [-t "
                 "<times.csv>] [-p] [mode options]\n"
              << "       depthmapXcli -v prints the current version\n"
              << "       depthmapXcli -h prints this help text\n"
              << "-s enables simple mode\n"
              << "-t <times.csv> enables output of runtimes as csv file\n"
              << "-p enables text progress printing\n"

              << "Possible modes are:\n";
    std::for_each(_parserFactory.getModeParsers().begin(), _parserFactory.getModeParsers().end(),
                  [](const ModeParserVec::value_type &p) -> void {
                      std::cout << "  " << p->getModeName() << "\n";
                  });
    std::cout << "\n";
    std::for_each(
        _parserFactory.getModeParsers().begin(), _parserFactory.getModeParsers().end(),
        [](const ModeParserVec::value_type &p) -> void { std::cout << p->getHelp() << "\n"; });
    std::cout << std::flush;
}

void CommandLineParser::printVersion() { std::cout << TITLE_BASE << "\n" << std::flush; }

CommandLineParser::CommandLineParser(const IModeParserFactory &parserFactory)
    : m_simpleMode(false), _parserFactory(parserFactory), _modeParser(0) {}

void CommandLineParser::parse(size_t argc, char *argv[]) {
    m_valid = false;
    m_printVersionMode = false;
    if (argc <= 1) {
        throw CommandLineException("No commandline parameters provided - don't know what to do");
    }
    for (size_t i = 1; i < argc;) {
        if (std::strcmp("-h", argv[i]) == 0) {
            return;
        } else if (std::strcmp("-v", argv[i]) == 0) {
            m_printVersionMode = true;
            return;
        } else if (std::strcmp("-m", argv[i]) == 0) {
            if (_modeParser) {
                throw CommandLineException("-m can only be used once");
            }
            ENFORCE_ARGUMENT("-m", i)

            for (auto iter = _parserFactory.getModeParsers().begin(),
                      end = _parserFactory.getModeParsers().end();
                 iter != end; ++iter) {
                if ((*iter)->getModeName() == argv[i]) {
                    _modeParser = iter->get();
                    break;
                }
            }

            if (!_modeParser) {
                throw CommandLineException(std::string("Invalid mode: ") + argv[i]);
            }
        } else if (std::strcmp("-f", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-f", i)
            m_fileName = argv[i];
        } else if (std::strcmp("-o", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-o", i)
            m_outputFile = argv[i];
        } else if (std::strcmp("-t", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-t", i)
            m_timingFile = argv[i];
        } else if (std::strcmp("-s", argv[i]) == 0) {
            m_simpleMode = true;
        } else if (std::strcmp("-p", argv[i]) == 0) {
            m_printProgress = true;
        }
        ++i;
    }

    if (!_modeParser) {
        throw CommandLineException("-m for mode is required");
    }
    if (m_fileName.empty()) {
        throw CommandLineException("-f for input file is required");
    }
    if (m_outputFile.empty()) {
        throw CommandLineException("-o for output file is required");
    }
    _modeParser->parse(argc, argv);
    m_valid = true;
}

void CommandLineParser::run(IPerformanceSink &perfWriter) const {
    if (!m_valid || !_modeParser) {
        throw CommandLineException("Trying to run with invalid command line parameters");
    }
    _modeParser->run(*this, perfWriter);
}
