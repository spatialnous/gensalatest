// Copyright (C) 2021, Petros Koutsolampros

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

#include "../derived/aglobjects.h"

#include "genlib/p2dpoly.h"

class AGLMap : public AGLObjects {

  protected:
    bool m_hoverStoreInvalid = false;
    bool m_hoverHasShapes = false;

  public:
    virtual ~AGLMap() {}
    virtual void updateHoverGL(bool m_core) = 0;
    virtual void highlightHoveredItems(const QtRegion &region) = 0;
};
