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

#include "gllinesuniform.h"
#include "glrastertexture.h"
#include "gltrianglesuniform.h"

#include "salalib/pointdata.h"

class GLPixelMap : public GLMap {
  public:
    GLPixelMap(PointMap &pointMap) : GLMap(), m_pixelMap(pointMap) {}
    void initializeGL(bool m_core) override {
        m_grid.initializeGL(m_core);
        m_rasterTexture.initializeGL(m_core);
        m_linkLines.initializeGL(m_core);
        m_linkFills.initializeGL(m_core);
        m_hoveredPixels.initializeGL(m_core);
    }

    void updateGL(bool m_core, bool reloadGLObjects) override {
        if (!m_datasetChanged)
            return;
        if (reloadGLObjects)
            loadGLObjects();
        loadGLObjectsRequiringGLContext();
        m_rasterTexture.updateGL(m_core);
        m_grid.updateGL(m_core);
        m_linkLines.updateGL(m_core);
        m_linkFills.updateGL(m_core);
        m_datasetChanged = false;
    }

    void updateHoverGL(bool m_core) override {
        if (m_hoverStoreInvalid) {
            m_hoveredPixels.updateGL(m_core);
            m_hoverStoreInvalid = false;
        }
    }

    void cleanup() override {
        m_grid.cleanup();
        m_rasterTexture.cleanup();
        m_linkLines.cleanup();
        m_linkFills.cleanup();
        m_hoveredPixels.cleanup();
    }

    void paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                 const QMatrix4x4 &m_mModel) override;
    void loadGLObjects() override;
    void loadGLObjectsRequiringGLContext() override;

    void highlightHoveredItems(const QtRegion &region) override { highlightHoveredPixels(region); }

    void setGridColour(QColor gridColour) { m_gridColour = gridColour; }
    void showLinks(bool showLinks) { m_showLinks = showLinks; }
    void showGrid(bool showGrid) { m_showGrid = showGrid; }
    void highlightHoveredPixels(const QtRegion &region);
    void highlightHoveredPixels(const std::set<PixelRef> &refs);

  private:
    PointMap &m_pixelMap;
    GLLinesUniform m_grid;
    GLRasterTexture m_rasterTexture;
    GLLinesUniform m_linkLines;
    GLTrianglesUniform m_linkFills;

    QColor m_gridColour =
        QColor::fromRgb((qRgb(255, 255, 255) & 0x006f6f6f) | (qRgb(0, 0, 0) & 0x00a0a0a0));

    bool m_showGrid = true;
    bool m_showLinks = false;

    GLLinesUniform m_hoveredPixels;
    PixelRef m_lastHoverPixel = -1;
};
