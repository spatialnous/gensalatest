// Copyright (C) 2017 Christian Sailer

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

#pragma once

#include "imodeparser.h"
#include "salalib/isovistdef.h"
#include <vector>

class IsovistParser : public IModeParser
{
public:
    IsovistParser();

    // IModeParser interface
public:
    std::string getModeName() const;
    std::string getHelp() const;
    void parse(size_t argc, char **argv);
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const;

    const std::vector<IsovistDefinition> &getIsovists() const{ return m_isovists;}
private:
    std::vector<IsovistDefinition> m_isovists;
};
