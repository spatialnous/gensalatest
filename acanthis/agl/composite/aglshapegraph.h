// Copyright (C) 2017, Petros Koutsolampros

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

#include "../composite/aglgraph.h"
#include "aglshapemap.h"

#include "salalib/shapegraph.h"

class AGLShapeGraph : public AGLShapeMap {
  public:
    AGLShapeGraph(ShapeGraph &shapeGraph, int pointSides, float pointRadius)
        : AGLShapeMap(shapeGraph, pointSides, pointRadius), m_shapeGraph(shapeGraph){};

    void initializeGL(bool core) override {
        AGLShapeMap::initializeGL(core);
        m_glGraph.initializeGL(core);
    }

    void updateGL(bool core) override {
        if (!m_datasetChanged)
            return;
        if (m_forceReloadGLObjects) {
            loadGLObjects();
            AGLShapeMap::loadGLObjects();
            m_glGraph.loadGLObjects();
            m_forceReloadGLObjects = false;
        }
        AGLShapeMap::updateGL(core);
        m_glGraph.updateGL(core);
        m_datasetChanged = false;
    }

    void cleanup() override {
        AGLShapeMap::cleanup();
        m_glGraph.cleanup();
    }

    void paintGL(const QMatrix4x4 &mProj, const QMatrix4x4 &mView,
                 const QMatrix4x4 &mModel) override {
        AGLShapeMap::paintGL(mProj, mView, mModel);
        if (m_showLinks) {
            m_glGraph.paintGL(mProj, mView, mModel);
        }
    }

    void showLinks(bool showLinks) { m_showLinks = showLinks; }
    void loadGLObjects() override;

  private:
    ShapeGraph &m_shapeGraph;

    AGLGraph m_glGraph;

    bool m_showLinks = true;
};
