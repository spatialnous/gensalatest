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

#include "aglmapviewrenderer.h"

#include <QOpenGLFunctions>
#include <QSettings>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickWindow>

class AGLMapView : public QQuickFramebufferObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GraphDocument *graphDocument //
                   MEMBER m_graphDocument WRITE setGraphDocument NOTIFY graphDocumentChanged)
    Q_PROPERTY(QColor foregroundColour //
                   MEMBER m_foregroundColour NOTIFY foregroundColourChanged)
    Q_PROPERTY(QColor backgroundColour //
                   MEMBER m_backgroundColour NOTIFY backgroundColourChanged)
    Q_PROPERTY(int antialiasingSamples //
                   MEMBER m_antialiasingSamples NOTIFY antialiasingSamplesChanged)
    Q_PROPERTY(bool highlightOnHover //
                   MEMBER m_highlightOnHover NOTIFY highlightOnHoverChanged)

    GraphDocument *m_graphDocument;
    QQuickFramebufferObject::Renderer *createRenderer() const override {
        connect(window(), &QQuickWindow::afterRendering, this, &AGLMapView::handleWindowSync,
                Qt::QueuedConnection);
        connect(this, &QQuickItem::widthChanged, this, &AGLMapView::forceUpdate,
                Qt::DirectConnection);
        connect(this, &QQuickItem::heightChanged, this, &AGLMapView::forceUpdate,
                Qt::DirectConnection);

        return new AGLMapViewRenderer(this, m_graphDocument, m_foregroundColour, m_backgroundColour,
                                      m_antialiasingSamples, m_highlightOnHover);
    }

  public:
    AGLMapView();
    void setGraphDocument(GraphDocument *graphDocument) {
        if (graphDocument == nullptr)
            return;
        m_graphDocument = graphDocument;
        matchViewToCurrentMetaGraph();

        emit graphDocumentChanged();
        setDirtyRenderer();
        //        update();
    }
    GraphDocument &getGraphDocument() const { return *m_graphDocument; }

    void panBy(int dx, int dy);
    void zoomBy(float dzf, int mouseX, int mouseY);
    void zoomToRegion(QtRegion region);
    void matchViewToCurrentMetaGraph();
    void zoomToSelection();

    void resetView();

    Point2f getWorldPoint(const QPoint &screenPoint);
    QPoint getScreenPoint(const Point2f &worldPoint);
    float getEyePosX() { return m_eyePosX; }
    float getEyePosY() { return m_eyePosY; }
    float getZoomFactor() { return m_zoomFactor; }
    QRectF getMouseDragRect() { return m_mouseDragRect; }

    void setModeJoin();
    void setModeUnjoin();

    void setModePan() { m_interactionMode = InteractionMode::PAN; }
    void setModeZoomIn() { m_interactionMode = InteractionMode::ZOOM_IN; }
    void setModeZoomOut() { m_interactionMode = InteractionMode::ZOOM_OUT; }

    void setModeFill();
    void setModeSemiFill();
    void setModeAugmentFill();
    void setModeSingleFill();

    void setModeIsovist();
    void setModeTargetedIsovist();

    void setModeSeedAxial();
    void setModeStepDepth();

    void setModeDrawLine();
    void setModeDrawPolygon();

    void setModeSelect();

    void OnEditCopy();
    void OnEditSave();

    void postLoadFile();

    void setDirtyRenderer() { m_dirtyRenderer = true; }

  signals:
    void foregroundColourChanged(const QColor &colour);
    void backgroundColourChanged();
    void antialiasingSamplesChanged();
    void highlightOnHoverChanged();
    void graphDocumentChanged();

  protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *object, QEvent *e) override;

  private slots:
    void handleWindowSync();
    void forceUpdate();

  private:
    QColor m_foregroundColour;
    QColor m_backgroundColour;
    int m_antialiasingSamples;
    bool m_highlightOnHover;

    // user interaction
    enum class InteractionMode {
        NONE,
        SELECT,
        PAN,
        ZOOM_IN,
        ZOOM_OUT,
        FILL_FULL,
        FILL_SEMI,
        FILL_AUGMENT,
        FILL_SINGLE,
        SEED_ISOVIST,
        SEED_TARGETED_ISOVIST,
        SEED_AXIAL,
        DRAW_LINE,
        DRAW_POLYGON,
        POINT_STEP_DEPTH,
        JOIN,
        UNJOIN
    };

    InteractionMode m_interactionMode = InteractionMode::SELECT;
    bool m_interactionSecondPoint = false;
    QPoint m_mouseLastPos;
    bool m_wasPanning = false;

    float m_eyePosX;
    float m_eyePosY;
    float m_minZoomFactor = 1;
    float m_zoomFactor = 20;
    float m_maxZoomFactor = 200;

    QRectF m_mouseDragRect = QRectF(0, 0, 0, 0);

    bool m_dirtyRenderer = false;
};
