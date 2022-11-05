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

#include "agldynamicline.h"
#include "agldynamicrect.h"
#include "agllines.h"

#include "graphdocument.h"

#include <QMatrix4x4>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLShaderProgram>
#include <QSettings>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickWindow>

class AGLMapView;
class AGLMapViewRenderer : public QQuickFramebufferObject::Renderer {

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        m_viewportSize = size;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(m_antialiasingSamples);
        return new QOpenGLFramebufferObject(size, format);
    }

    void synchronize(QQuickFramebufferObject *item) override;

  public:
    AGLMapViewRenderer(const QQuickFramebufferObject *item, const GraphDocument *pDoc,
                       const QColor &foregrounColour, const QColor &backgroundColour,
                       int antialiasingSamples, bool highlightOnHover);
    ~AGLMapViewRenderer();

    void render() override;

  private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program = nullptr;
    const AGLMapView *m_item;

    void recalcView();

    static QColor colorMerge(QColor color, QColor mergecolor) {
        return QColor::fromRgb((color.rgba() & 0x006f6f6f) | (mergecolor.rgba() & 0x00a0a0a0));
    }

    void notifyDatasetChanged() {
        m_datasetChanged = true;
        //        update();
    }

    QString m_currentFile;

    bool m_core;
    bool m_perspectiveView = false;
    float m_eyePosX;
    float m_eyePosY;
    float m_zoomFactor = 20;
    QMatrix4x4 m_mProj;
    QMatrix4x4 m_mView;
    QMatrix4x4 m_mModel;

    QColor m_foregroundColour;
    QColor m_backgroundColour;

    AGLDynamicRect m_selectionRect;
    AGLDynamicLine m_dragLine;
    AGLLines m_axes;

    bool m_highlightOnHover = true;

    int m_antialiasingSamples = 0; // set this to 0 if rendering is too slow

    bool m_datasetChanged = false;

    void highlightHoveredItems(const QtRegion &region);

    void loadAxes();

    QRectF m_mouseDragRect = QRectF(0, 0, 0, 0);

    Point2f m_tempFirstPoint;
    Point2f m_tempSecondPoint;

    int m_currentlyEditingShapeRef = -1;

    Point2f m_polyStart;
    int m_polyPoints = 0;

    inline int PixelDist(QPoint a, QPoint b) {
        return (int)sqrt(
            double((b.x() - a.x()) * (b.x() - a.x()) + (b.y() - a.y()) * (b.y() - a.y())));
    }

    // MetaGraph &getMetaGraph();
    std::vector<std::unique_ptr<MapLayer>> &getMaps();
};
