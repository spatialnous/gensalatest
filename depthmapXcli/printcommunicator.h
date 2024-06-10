// Copyright (C) 2020, Petros Koutsolampros

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

#include "genlib/comm.h"

class PrintCommunicator : public ICommunicator {
  public:
    PrintCommunicator() {
        num_steps = 0;
        step = 0;
        num_records = 0;
        record = 0;
    }
    virtual ~PrintCommunicator() {}
    virtual void CommPostMessage(size_t m, size_t x) const;
};
