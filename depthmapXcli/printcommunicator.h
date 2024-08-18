// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "genlib/comm.h"

class PrintCommunicator : public ICommunicator {
  public:
    PrintCommunicator() {
        m_numSteps = 0;
        m_step = 0;
        m_numRecords = 0;
        m_record = 0;
    }
    virtual ~PrintCommunicator() {}
    virtual void CommPostMessage(size_t m, size_t x) const;
};
