// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
