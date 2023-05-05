// Copyright (C) 2022, Petros Koutsolampros

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

#include "aglmodel.h"

#include "../composite/aglmap.h"

#include "maplayer.h"


class AGLMapsModel : public AGLModel {
    AGLMap &getGLMap(MapLayer *mapLayer);
    std::map<MapLayer *, std::unique_ptr<AGLMap>> m_glMaps;

public:
    using AGLModel::AGLModel;
    const QList<QSharedPointer<MapLayer> > &getMaps() const;
    void cleanup() override;
    void loadGLObjects() override;
    void initializeGL(bool m_core) override;
    void loadGLObjectsRequiringGLContext() override;
    void updateGL(bool m_core) override;
    void paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                 const QMatrix4x4 &m_mModel) override;

    void highlightHoveredItems(const QtRegion &region);
};
