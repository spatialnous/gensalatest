// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdio.h>
#include <string>

class SelfCleaningFile {
  public:
    SelfCleaningFile(const std::string &filename) : _filename(filename) {}
    ~SelfCleaningFile() { std::remove(_filename.c_str()); }

    const std::string &Filename() { return _filename; }

  private:
    const std::string _filename;
};
