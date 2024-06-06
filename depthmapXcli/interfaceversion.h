// Copyright (C) 2017 Christian Sailer
// Copyright (C) 2021 Petros Koutsolampros

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

#include "version_defs.h"

#include "../version.h"

#define INTERFACE_MAJOR_VERSION DEPTHMAPX_MAJOR_VERSION
#define INTERFACE_MINOR_VERSION DEPTHMAPX_MINOR_VERSION
#define INTERFACE_REVISION_VERSION DEPTHMAPX_REVISION_VERSION

// leave these alone - C Preprocessor magic to get stuff to the right format
#define TITLE_BASE                                                                                 \
    TITLE_BASE_FORMAT("depthmapXcli", INTERFACE_MAJOR_VERSION, INTERFACE_MINOR_VERSION,            \
                      INTERFACE_REVISION_VERSION)
