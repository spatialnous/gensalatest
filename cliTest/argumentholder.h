// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>

class ArgumentHolder {
  public:
    ArgumentHolder(std::initializer_list<std::string> l) : mArguments(l) {
        for (auto &arg : mArguments) {
            mArgv.push_back(arg.data());
        }
    }

    char **argv() const { return (char **)mArgv.data(); }

    size_t argc() const { return mArgv.size(); }

  private:
    std::vector<std::string> mArguments;
    std::vector<const char *> mArgv;
};
