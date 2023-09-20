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

#include "graphviewmodel.h"

#include <QOpenGLFunctions>
#include <QSettings>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickWindow>

class AGLMapViewport : public QQuickFramebufferObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GraphViewModel *graphViewModel //
               MEMBER m_graphViewModel
               WRITE setGraphViewModel
               NOTIFY graphViewModelChanged)
    Q_PROPERTY(QColor foregroundColour //
               MEMBER m_foregroundColour
               NOTIFY foregroundColourChanged)
    Q_PROPERTY(QColor backgroundColour //
               MEMBER m_backgroundColour
               NOTIFY backgroundColourChanged)
    Q_PROPERTY(int antialiasingSamples //
               MEMBER m_antialiasingSamples
               NOTIFY antialiasingSamplesChanged)
    Q_PROPERTY(bool highlightOnHover //
               MEMBER m_highlightOnHover
               NOTIFY highlightOnHoverChanged)

    GraphViewModel *m_graphViewModel = nullptr;
    QQuickFramebufferObject::Renderer *createRenderer() const override {
        connect(window(), &QQuickWindow::afterRendering, this, &AGLMapViewport::handleWindowSync,
                Qt::QueuedConnection);
        connect(this, &QQuickItem::widthChanged, this, &AGLMapViewport::forceUpdate,
                Qt::DirectConnection);
        connect(this, &QQuickItem::heightChanged, this, &AGLMapViewport::forceUpdate,
                Qt::DirectConnection);
        connect(this, &QQuickItem::heightChanged, this, &AGLMapViewport::forceUpdate,
                Qt::DirectConnection);

        return new AGLMapViewRenderer(this, m_graphViewModel,
                                      m_foregroundColour, m_backgroundColour,
                                      m_antialiasingSamples, m_highlightOnHover);
    }

  public:
    AGLMapViewport();
    void setGraphViewModel(GraphViewModel *graphViewModel) {
        if (graphViewModel == nullptr)
            return;
        m_graphViewModel = graphViewModel;
        matchViewToCurrentMetaGraph();

        emit graphViewModelChanged();
        setDirtyRenderer();
        //        update();
    }
    GraphViewModel &getGraphViewModel() const { return *m_graphViewModel; }

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
    QColor getForegroundColour() { return m_foregroundColour; }
    QColor getBackgroundColour() { return m_backgroundColour; }

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

    Q_INVOKABLE void setDirtyRenderer() { m_dirtyRenderer = true; }

  signals:
    void foregroundColourChanged(const QColor &colour);
    void backgroundColourChanged(const QColor &colour);
    void antialiasingSamplesChanged();
    void highlightOnHoverChanged();
    void graphViewModelChanged();
    void mousePressed();

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
