// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "printcommunicator.h"

#include <iostream>

void PrintCommunicator::CommPostMessage(size_t m, size_t x) const {
    switch (m) {
    case Communicator::NUM_STEPS:
        num_steps = x;
        break;
    case Communicator::CURRENT_STEP:
        step = x;
        break;
    case Communicator::NUM_RECORDS:
        num_records = x;
        break;
    case Communicator::CURRENT_RECORD:
        record = x;
        if (record > num_records)
            record = num_records;
        std::cout << "step: " << step << "/" << num_steps << " "
                  << "record: " << record << "/" << num_records << std::endl;
        break;
    default:
        break;
    }
}
