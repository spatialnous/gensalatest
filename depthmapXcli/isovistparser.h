// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.h"

#include "salalib/isovistdef.h"

#include <vector>

class IsovistParser : public IModeParser {
  public:
    IsovistParser();

    // IModeParser interface
  public:
    std::string getModeName() const;
    std::string getHelp() const;
    void parse(size_t argc, char **argv);
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const;

    const std::vector<IsovistDefinition> &getIsovists() const { return m_isovists; }

  private:
    std::vector<IsovistDefinition> m_isovists;
};
