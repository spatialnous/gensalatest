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

#include "aglpixelmap.h"

#include "salalib/geometrygenerators.h"
#include "salalib/linkutils.h"

void AGLPixelMap::loadGLObjects() {
    QtRegion region = m_pixelMap.getRegion();
    m_rasterTexture.loadRegionData(region.bottom_left.x, region.bottom_left.y, region.top_right.x,
                                   region.top_right.y);

    if (m_showGrid) {
        std::vector<SimpleLine> gridData;
        double spacing = m_pixelMap.getSpacing();
        double offsetX = region.bottom_left.x;
        double offsetY = region.bottom_left.y;
        for (int x = 1; x < m_pixelMap.getCols(); x++) {
            gridData.push_back(SimpleLine(offsetX + x * spacing, region.bottom_left.y,
                                          offsetX + x * spacing, region.top_right.y));
        }
        for (int y = 1; y < m_pixelMap.getRows(); y++) {
            gridData.push_back(SimpleLine(region.bottom_left.x, offsetY + y * spacing,
                                          region.top_right.x, offsetY + y * spacing));
        }
        m_grid.loadLineData(gridData, m_gridColour);
    }
    if (m_showLinks) {
        const std::vector<SimpleLine> &mergedPixelLines =
            depthmapX::getMergedPixelsAsLines(m_pixelMap);
        std::vector<Point2f> mergedPixelLocations;
        for (auto &mergeLine : mergedPixelLines) {
            mergedPixelLocations.push_back(mergeLine.start());
            mergedPixelLocations.push_back(mergeLine.end());
        }

        const std::vector<Point2f> &linkFillTriangles =
            GeometryGenerators::generateMultipleDiskTriangles(32, m_pixelMap.getSpacing() * 0.25,
                                                              mergedPixelLocations);
        m_linkFills.loadTriangleData(linkFillTriangles, qRgb(0, 0, 0));

        std::vector<SimpleLine> linkFillPerimeters =
            GeometryGenerators::generateMultipleCircleLines(32, m_pixelMap.getSpacing() * 0.25,
                                                            mergedPixelLocations);
        linkFillPerimeters.insert(linkFillPerimeters.end(), mergedPixelLines.begin(),
                                  mergedPixelLines.end());
        m_linkLines.loadLineData(linkFillPerimeters, qRgb(0, 255, 0));
    }
}
void AGLPixelMap::loadGLObjectsRequiringGLContext() {
    QImage data(m_pixelMap.getCols(), m_pixelMap.getRows(), QImage::Format_RGBA8888);
    data.fill(Qt::transparent);

    for (int y = 0; y < m_pixelMap.getRows(); y++) {
        for (int x = 0; x < m_pixelMap.getCols(); x++) {
            PixelRef pix(x, y);
            PafColor colour = m_pixelMap.getPointColor(pix);
            if (colour.alphab() != 0) { // alpha == 0 is transparent
                data.setPixelColor(x, y, qRgb(colour.redb(), colour.greenb(), colour.blueb()));
            }
        }
    }
    m_rasterTexture.loadPixelData(data);
}

void AGLPixelMap::paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                          const QMatrix4x4 &m_mModel) {
    m_rasterTexture.paintGL(m_mProj, m_mView, m_mModel);
    if (m_showGrid)
        m_grid.paintGL(m_mProj, m_mView, m_mModel);
    if (m_showLinks) {
        QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
        glFuncs->glLineWidth(3);
        m_linkLines.paintGL(m_mProj, m_mView, m_mModel);
        m_linkFills.paintGL(m_mProj, m_mView, m_mModel);
        glFuncs->glLineWidth(1);
    }
    glLineWidth(3);
    m_hoveredPixels.paintGL(m_mProj, m_mView, m_mModel);
    glLineWidth(1);
}

void AGLPixelMap::highlightHoveredPixels(const QtRegion &region) {
    // n.b., assumes constrain set to true (for if you start the selection off the
    // grid)
    PixelRef s_bl = m_pixelMap.pixelate(region.bottom_left, true);
    PixelRef s_tr = m_pixelMap.pixelate(region.top_right, true);
    std::vector<Point> points;
    PixelRef hoverPixel = -1;
    for (short i = s_bl.x; i <= s_tr.x; i++) {
        for (short j = s_bl.y; j <= s_tr.y; j++) {
            PixelRef ref = PixelRef(i, j);
            if (m_pixelMap.includes(ref)) {
                const Point &p = m_pixelMap.getPoint(ref);
                if (p.filled()) {
                    points.push_back(p);
                    hoverPixel = ref;
                }
            }
        }
    }

    if (!points.empty()) {
        // do not redo the whole thing if we are still hovering the same pixel
        if (points.size() == 1 && hoverPixel == m_lastHoverPixel)
            return;
        std::vector<SimpleLine> lines;
        int i = 0;
        for (Point point : points) {
            const Point2f &loc = point.getLocation();
            lines.push_back(SimpleLine(
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5,
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5,
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5,
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5,
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5));
            i++;
        }
        m_hoveredPixels.loadLineData(lines, qRgb(255, 255, 0));
        m_hoverStoreInvalid = true;
        m_hoverHasShapes = true;
    } else if (m_hoverHasShapes) {
        m_hoveredPixels.loadLineData(std::vector<SimpleLine>(), qRgb(255, 255, 0));
        m_hoverStoreInvalid = true;
        m_hoverHasShapes = false;
    }

    if (m_hoverStoreInvalid) {
        //        update();
    }
}

void AGLPixelMap::highlightHoveredPixels(const std::set<PixelRef> &refs) {
    // n.b., assumes constrain set to true (for if you start the selection off the
    // grid)
    std::vector<Point> points;
    PixelRef hoverPixel = -1;
    for (PixelRef ref : refs) {
        if (m_pixelMap.includes(ref)) {
            const Point &p = m_pixelMap.getPoint(ref);
            if (p.filled()) {
                points.push_back(p);
                hoverPixel = ref;
            }
        }
    }

    if (!points.empty()) {
        // do not redo the whole thing if we are still hovering the same pixel
        if (points.size() == 1 && hoverPixel == m_lastHoverPixel)
            return;
        std::vector<SimpleLine> lines;
        int i = 0;
        for (Point point : points) {
            const Point2f &loc = point.getLocation();
            lines.push_back(SimpleLine(
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5,
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5,
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y + m_pixelMap.getSpacing() * 0.5,
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5));
            lines.push_back(SimpleLine(
                loc.x + m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5,
                loc.x - m_pixelMap.getSpacing() * 0.5, loc.y - m_pixelMap.getSpacing() * 0.5));
            i++;
        }
        m_hoveredPixels.loadLineData(lines, qRgb(255, 255, 0));
        m_hoverStoreInvalid = true;
        m_hoverHasShapes = true;
    } else if (m_hoverHasShapes) {
        m_hoveredPixels.loadLineData(std::vector<SimpleLine>(), qRgb(255, 255, 0));
        m_hoverStoreInvalid = true;
        m_hoverHasShapes = false;
    }

    if (m_hoverStoreInvalid) {
        //        update();
    }
}
