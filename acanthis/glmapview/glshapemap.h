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

#include "glmap.h"

#include "gllines.h"
#include "gllinesuniform.h"
#include "glpolygons.h"
#include "glregularpolygons.h"
#include "salalib/mgraph.h"

class GLShapeMap : public GLMap {
  public:
    GLShapeMap(ShapeMap &shapeMap, int pointSides, float pointRadius)
        : GLMap(GLMap::GLMapType::SHAPEMAP), m_shapeMap(shapeMap),
          m_pointSides(pointSides), m_pointRadius(pointRadius){};
    GLShapeMap(GLMap::GLMapType mapType, ShapeMap &shapeMap, int pointSides,
               float pointRadius)
        : GLMap(mapType), m_shapeMap(shapeMap), m_pointSides(pointSides),
          m_pointRadius(pointRadius){};

    void initializeGL(bool m_core) override {
        m_lines.initializeGL(m_core);
        m_polygons.initializeGL(m_core);
        m_points.initializeGL(m_core);
        m_hoveredShapes.initializeGL(m_core);
    }

    void updateGL(bool m_core, bool reloadGLObjects) override {
        if (!m_datasetChanged)
            return;
        if (reloadGLObjects)
            loadGLObjects();
        m_lines.updateGL(m_core);
        m_polygons.updateGL(m_core);
        m_points.updateGL(m_core);
        m_datasetChanged = false;
    }

    void updateHoverGL(bool m_core) override {
        if (m_hoverStoreInvalid) {
            m_hoveredShapes.updateGL(m_core);
            m_hoverStoreInvalid = false;
        }
    }

    void cleanup() override {
        m_lines.cleanup();
        m_polygons.cleanup();
        m_points.cleanup();
        m_hoveredShapes.cleanup();
    }

    void paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                 const QMatrix4x4 &m_mModel) override {
        m_lines.paintGL(m_mProj, m_mView, m_mModel);
        m_polygons.paintGL(m_mProj, m_mView, m_mModel);
        m_points.paintGL(m_mProj, m_mView, m_mModel);
        glLineWidth(10);
        m_hoveredShapes.paintGL(m_mProj, m_mView, m_mModel);
        glLineWidth(1);
    }

    void loadGLObjects() override;
    void highlightHoveredItems(const QtRegion &region) override {
        highlightHoveredShapes(region);
    };

    void highlightHoveredShapes(const QtRegion &region);

  private:
    ShapeMap &m_shapeMap;
    GLLines m_lines;
    GLPolygons m_polygons;
    GLRegularPolygons m_points;
    GLLines m_hoveredShapes;
    const int m_pointSides;
    const float m_pointRadius;
};
