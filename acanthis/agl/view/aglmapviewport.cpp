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

#include "aglmapviewport.h"

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QtCore/QRunnable>

AGLMapViewport::AGLMapViewport() : m_eyePosX(0), m_eyePosY(0) {
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    // renderer is initially created as "dirty" because two passes are
    // necessary for it to provide a clean OpenGL context.
    m_dirtyRenderer = true;
}

void AGLMapViewport::handleWindowSync() {
    if (m_dirtyRenderer) {
        m_dirtyRenderer = false;
        update();
    }
}

void AGLMapViewport::forceUpdate() { update(); setDirtyRenderer(); }

void AGLMapViewport::mouseReleaseEvent(QMouseEvent *event) {
    if (m_wasPanning) {
        m_wasPanning = false;
        return;
    }
    QPoint mousePoint = event->pos();
    Point2f worldPoint = getWorldPoint(mousePoint);
    //    if (!m_pDoc.m_communicator) {
    //        QtRegion r;
    //        if (m_mouseDragRect.isNull()) {
    //            r.bottom_left = worldPoint;
    //            r.top_right = worldPoint;
    //        } else {
    //            r.bottom_left.x = std::min(m_mouseDragRect.bottomRight().x(),
    //            m_mouseDragRect.topLeft().x()); r.bottom_left.y =
    //            std::min(m_mouseDragRect.bottomRight().y(),
    //            m_mouseDragRect.topLeft().y()); r.top_right.x =
    //            std::max(m_mouseDragRect.bottomRight().x(),
    //            m_mouseDragRect.topLeft().x()); r.top_right.y =
    //            std::max(m_mouseDragRect.bottomRight().y(),
    //            m_mouseDragRect.topLeft().y());
    //        }
    //        bool selected = false;
    //        switch (m_interactionMode) {
    //        case InteractionMode::NONE: {
    //            // nothing, deselect
    //            m_pDoc.getMetaGraph().clearSel();
    //            break;
    //        }
    //        case InteractionMode::SELECT: {
    //            // typical selection
    //            Qt::KeyboardModifiers keyMods = QApplication::keyboardModifiers();
    //            m_pDoc.getMetaGraph().setCurSel(r, keyMods & Qt::ShiftModifier);
    //            ((MainWindow *)m_pDoc.m_mainFrame)->updateToolbar();
    //            if (m_highlightOnHover) highlightHoveredItems(r);
    //            break;
    //        }
    //        case InteractionMode::ZOOM_IN: {
    //            if (r.width() > 0) {
    //                OnViewZoomToRegion(r);
    //                recalcView();
    //            } else {
    //                zoomBy(0.8, mousePoint.x(), mousePoint.y());
    //            }
    //            break;
    //        }
    //        case InteractionMode::ZOOM_OUT: {
    //            zoomBy(1.2, mousePoint.x(), mousePoint.y());
    //            break;
    //        }
    //        case InteractionMode::FILL_FULL: {
    //            m_pDoc.OnFillPoints(worldPoint, 0);
    //            break;
    //        }
    //        case InteractionMode::PENCIL: {
    //            m_pDoc.getMetaGraph().getDisplayedPointMap().fillPoint(worldPoint,
    //            event->button() == Qt::LeftButton); break;
    //        }
    //        case InteractionMode::SEED_ISOVIST: {
    //            m_pDoc.OnMakeIsovist(worldPoint);
    //            break;
    //        }
    //        case InteractionMode::SEED_TARGETED_ISOVIST: {
    //            m_tempFirstPoint = worldPoint;
    //            m_tempSecondPoint = worldPoint;
    //            m_interactionMode = InteractionMode::SEED_TARGETED_ISOVIST |
    //            InteractionMode::SECOND_POINT; break;
    //        }
    //        case InteractionMode::SEED_TARGETED_ISOVIST | InteractionMode::SECOND_POINT:
    //        {
    //            Line directionLine(m_tempFirstPoint, worldPoint);
    //            Point2f vec = directionLine.vector();
    //            vec.normalise();
    //            m_pDoc.OnMakeIsovist(m_tempFirstPoint, vec.angle());
    //            m_interactionMode = InteractionMode::SEED_TARGETED_ISOVIST;
    //            break;
    //        }
    //        case InteractionMode::SEED_AXIAL: {
    //            m_pDoc.OnToolsAxialMap(worldPoint);
    //            break;
    //        }
    //        case InteractionMode::LINE_TOOL: {
    //            m_tempFirstPoint = worldPoint;
    //            m_tempSecondPoint = worldPoint;
    //            m_interactionMode = InteractionMode::LINE_TOOL |
    //            InteractionMode::SECOND_POINT; break;
    //        }
    //        case InteractionMode::LINE_TOOL | InteractionMode::SECOND_POINT: {
    //            if (m_pDoc.getMetaGraph().makeShape(Line(m_tempFirstPoint, worldPoint)))
    //            {
    //                m_pDoc.modifiedFlag = true;
    //                m_pDoc.SetRedrawFlag(QGraphDoc::VIEW_ALL, QGraphDoc::REDRAW_GRAPH,
    //                QGraphDoc::NEW_DATA);
    //            }
    //            m_tempFirstPoint = worldPoint;
    //            m_tempSecondPoint = worldPoint;
    //            m_interactionMode = InteractionMode::LINE_TOOL;

    //            break;
    //        }
    //        case InteractionMode::POLYGON_TOOL: {
    //            m_tempFirstPoint = worldPoint;
    //            m_tempSecondPoint = worldPoint;
    //            m_polyPoints = 0;
    //            m_interactionMode = InteractionMode::POLYGON_TOOL |
    //            InteractionMode::SECOND_POINT; break;
    //        }
    //        case InteractionMode::POLYGON_TOOL | InteractionMode::SECOND_POINT: {
    //            if (m_polyPoints == 0) {
    //                m_currentlyEditingShapeRef =
    //                m_pDoc.getMetaGraph().polyBegin(Line(m_tempFirstPoint, worldPoint));
    //                m_polyStart = m_tempFirstPoint;
    //                m_tempFirstPoint = m_tempSecondPoint;
    //                m_polyPoints += 2;
    //            } else if (m_polyPoints > 2 && PixelDist(mousePoint,
    //            getScreenPoint(m_polyStart)) < 6) {
    //                // check to see if it's back to the original start point, if so,
    //                close off
    //                m_pDoc.getMetaGraph().polyClose(m_currentlyEditingShapeRef);
    //                m_polyPoints = 0;
    //                m_currentlyEditingShapeRef = -1;
    //                m_interactionMode = InteractionMode::POLYGON_TOOL;
    //            } else {
    //                m_pDoc.getMetaGraph().polyAppend(m_currentlyEditingShapeRef,
    //                worldPoint); m_tempFirstPoint = m_tempSecondPoint; m_polyPoints +=
    //                1;
    //            }
    //            break;
    //        }
    //        case InteractionMode::POINT_STEP_DEPTH: {
    //            m_pDoc.getMetaGraph().setCurSel(r, false);
    //            m_pDoc.OnToolsPD();
    //            break;
    //        }
    //        case InteractionMode::JOIN: {
    //            selected = m_pDoc.getMetaGraph().setCurSel(r, false);
    //            int selectedCount = m_pDoc.getMetaGraph().getSelCount();
    //            if (selectedCount > 0) {
    //                Point2f selectionCentre;
    //                if (selectedCount > 1) {
    //                    QtRegion selBounds = m_pDoc.getMetaGraph().getSelBounds();
    //                    selectionCentre.x = (selBounds.bottom_left.x +
    //                    selBounds.top_right.x) * 0.5; selectionCentre.y =
    //                    (selBounds.bottom_left.y + selBounds.top_right.y) * 0.5;
    //                } else {
    //                    const std::set<int> &selectedSet =
    //                    m_pDoc.getMetaGraph().getSelSet(); if
    //                    (m_pDoc.getMetaGraph().getViewClass() & MetaGraph::VIEWVGA) {
    //                        selectionCentre =
    //                        m_pDoc.getMetaGraph().getDisplayedPointMap().depixelate(*selectedSet.begin());
    //                    } else if (m_pDoc.getMetaGraph().getViewClass() &
    //                    MetaGraph::VIEWAXIAL) {
    //                        selectionCentre =
    //                        m_pDoc.getMetaGraph().getDisplayedShapeGraph()
    //                                              .getAllShapes()[*selectedSet.begin()]
    //                                              .getCentroid();
    //                    }
    //                }
    //                m_tempFirstPoint = selectionCentre;
    //                m_tempSecondPoint = selectionCentre;
    //                m_interactionMode = InteractionMode::JOIN |
    //                InteractionMode::SECOND_POINT;
    //            }
    //            break;
    //        }
    //        case InteractionMode::JOIN | InteractionMode::SECOND_POINT: {
    //            int selectedCount = m_pDoc.getMetaGraph().getSelCount();
    //            if (selectedCount > 0) {
    //                if (m_pDoc.getMetaGraph().getViewClass() & MetaGraph::VIEWVGA) {
    //                    m_pDoc.getMetaGraph().getDisplayedPointMap().mergePoints(worldPoint);
    //                } else if (m_pDoc.getMetaGraph().getViewClass() &
    //                MetaGraph::VIEWAXIAL && selectedCount == 1) {
    //                    m_pDoc.getMetaGraph().setCurSel(r, true); // add the new one to
    //                    the selection set const auto &selectedSet =
    //                    m_pDoc.getMetaGraph().getSelSet(); if (selectedSet.size() == 2)
    //                    {
    //                        std::set<int>::iterator it = selectedSet.begin();
    //                        int axRef1 = *it;
    //                        it++;
    //                        int axRef2 = *it;
    //                        // axial is only joined one-by-one
    //                        m_pDoc.modifiedFlag = true;
    //                        m_pDoc.getMetaGraph().getDisplayedShapeGraph().linkShapesFromRefs(axRef1,
    //                        axRef2, true); m_pDoc.getMetaGraph().clearSel();
    //                    }
    //                }
    //                m_pDoc.getMetaGraph().clearSel();
    //                m_interactionMode = InteractionMode::JOIN;
    //            }
    //            break;
    //        }
    //        case InteractionMode::UNJOIN: {
    //            m_pDoc.getMetaGraph().setCurSel(r, false);
    //            int selectedCount = m_pDoc.getMetaGraph().getSelCount();
    //            if (selectedCount > 0) {
    //                if (m_pDoc.getMetaGraph().getViewClass() & MetaGraph::VIEWVGA) {
    //                    if
    //                    (m_pDoc.getMetaGraph().getDisplayedPointMap().unmergePoints()) {
    //                        m_pDoc.modifiedFlag = true;
    //                        m_pDoc.SetRedrawFlag(QGraphDoc::VIEW_ALL,
    //                        QGraphDoc::REDRAW_GRAPH, QGraphDoc::NEW_DATA);
    //                    }
    //                } else if (m_pDoc.getMetaGraph().getViewClass() &
    //                MetaGraph::VIEWAXIAL) {
    //                    const auto &selectedSet = m_pDoc.getMetaGraph().getSelSet();
    //                    Point2f selectionCentre =
    //                    m_pDoc.getMetaGraph().getDisplayedShapeGraph()
    //                                                  .getAllShapes()[*selectedSet.begin()]
    //                                                  .getCentroid();
    //                    m_tempFirstPoint = selectionCentre;
    //                    m_tempSecondPoint = selectionCentre;
    //                    m_interactionMode = InteractionMode::UNJOIN |
    //                    InteractionMode::SECOND_POINT;
    //                }
    //            }
    //            break;
    //        }
    //        case InteractionMode::UNJOIN | InteractionMode::SECOND_POINT: {
    //            int selectedCount = m_pDoc.getMetaGraph().getSelCount();
    //            if (selectedCount > 0) {
    //                if (m_pDoc.getMetaGraph().getViewClass() & MetaGraph::VIEWAXIAL &&
    //                selectedCount == 1) {
    //                    m_pDoc.getMetaGraph().setCurSel(r, true); // add the new one to
    //                    the selection set const auto &selectedSet =
    //                    m_pDoc.getMetaGraph().getSelSet(); if (selectedSet.size() == 2)
    //                    {
    //                        std::set<int>::iterator it = selectedSet.begin();
    //                        int axRef1 = *it;
    //                        it++;
    //                        int axRef2 = *it;
    //                        // axial is only joined one-by-one
    //                        m_pDoc.modifiedFlag = true;
    //                        m_pDoc.getMetaGraph().getDisplayedShapeGraph().unlinkShapesFromRefs(axRef1,
    //                        axRef2, true); m_pDoc.getMetaGraph().clearSel();
    //                    }
    //                }
    //                m_pDoc.getMetaGraph().clearSel();
    //                m_interactionMode = InteractionMode::UNJOIN;
    //            }
    //            break;
    //        }
    //        }

    //        m_pDoc.SetRedrawFlag(QGraphDoc::VIEW_ALL, QGraphDoc::REDRAW_POINTS,
    //        QGraphDoc::NEW_SELECTION);
    //    }
    m_mouseDragRect.setWidth(0);
    m_mouseDragRect.setHeight(0);
    update();
}

void AGLMapViewport::mousePressEvent(QMouseEvent *event) {
    emit mousePressed();
    std::cout << "click" << std::endl;
    m_mouseLastPos = event->pos();
}

void AGLMapViewport::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->position().x() - m_mouseLastPos.x();
    int dy = event->position().y() - m_mouseLastPos.y();

    Point2f worldPoint = getWorldPoint(event->pos());

    //    if (m_mouseDragRect.isNull() &&
    //        !(m_interactionSecondPoint && m_pDoc.getMetaGraph().getViewClass() &
    //        MetaGraph::VIEWVGA)) {
    // if (m_highlightOnHover) highlightHoveredItems(QtRegion(worldPoint,
    //        worldPoint));
    //    }

    if (event->buttons() & Qt::RightButton ||
        (event->buttons() & Qt::LeftButton && m_interactionMode == InteractionMode::PAN)) {
        panBy(dx, dy);
        m_wasPanning = true;
    } else if (event->buttons() & Qt::LeftButton) {
        Point2f lastWorldPoint = getWorldPoint(m_mouseLastPos);

        if (m_mouseDragRect.isNull()) {
            m_mouseDragRect.setX(lastWorldPoint.x);
            m_mouseDragRect.setY(lastWorldPoint.y);
        }

        m_mouseDragRect.setWidth(worldPoint.x - m_mouseDragRect.x());
        m_mouseDragRect.setHeight(worldPoint.y - m_mouseDragRect.y());

        //        QtRegion hoverRegion;
        //        hoverRegion.bottom_left.x = std::min(m_mouseDragRect.bottomRight().x(),
        //        m_mouseDragRect.topLeft().x()); hoverRegion.bottom_left.y =
        //        std::min(m_mouseDragRect.bottomRight().y(),
        //        m_mouseDragRect.topLeft().y()); hoverRegion.top_right.x =
        //        std::max(m_mouseDragRect.bottomRight().x(),
        //        m_mouseDragRect.topLeft().x()); hoverRegion.top_right.y =
        //        std::max(m_mouseDragRect.bottomRight().y(),
        //        m_mouseDragRect.topLeft().y());
        // if (m_highlightOnHover) highlightHoveredItems(hoverRegion);
        //        update();
        //    }
        //    if ((m_interactionMode & InteractionMode::SECOND_POINT) ==
        //    InteractionMode::SECOND_POINT) {
        //        m_tempSecondPoint = worldPoint;
        //        if (m_highlightOnHover && m_pDoc.getMetaGraph().getViewClass() &
        //        MetaGraph::VIEWVGA) {
        //            PointMap &map = m_pDoc.getMetaGraph().getDisplayedPointMap();
        //            QtRegion selectionBounds = map.getSelBounds();
        //            PixelRef worldPixel = map.pixelate(worldPoint, true);
        //            PixelRef boundsPixel =
        //                map.pixelate(Point2f(selectionBounds.top_right.x,
        //                selectionBounds.bottom_left.y), true);
        //            std::set<int> &selection = map.getSelSet();
        //            std::set<PixelRef> offsetSelection;
        //            for (int ref : selection) {
        //                PixelRef pixelRef = PixelRef(ref) + worldPixel - boundsPixel;
        //                offsetSelection.insert(pixelRef);
        //            }
        //            highlightHoveredPixels(map, offsetSelection);
        //        }
        //        update();
    }
    update();
    m_mouseLastPos = event->pos();
    //    m_pDoc.m_position = worldPoint;
    //    m_pDoc.UpdateMainframestatus();
}

void AGLMapViewport::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    int x = event->position().x();
    int y = event->position().y();

    zoomBy(1 - 0.25f * numDegrees.y() / 15.0f, x, y);

    update();

    event->accept();
}

bool AGLMapViewport::eventFilter(QObject *object, QEvent *e) {
    if (e->type() == QEvent::ToolTip) {

        //    if (!m_pDoc.m_communicator) {
        //        if (m_pDoc.m_meta_graph) {
        //            if (m_pDoc.getMetaGraph().viewingProcessed() &&
        //            m_pDoc.getMetaGraph().getSelCount() > 1) {
        //                float val = m_pDoc.getMetaGraph().getSelAvg();
        //                int count = m_pDoc.getMetaGraph().getSelCount();
        //                if (val == -1.0f)
        //                    return "Null selection";
        //                else if (val != -2.0f)
        //                    return (QString("Selection\nAverage: %1\nCount:
        //                    %2").arg(val).arg(count)).toStdString();
        //                else
        //                    return "";
        //            } else if (m_pDoc.getMetaGraph().viewingProcessed()) {
        //                // and that it has an appropriate state to display a hover wnd
        //                QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e); // Tool
        //                tip events come as the type QHelpEvent float val =
        //                m_pDoc.getMetaGraph().getLocationValue(getWorldPoint(helpEvent->pos()));
        //                if (val == -1.0f)
        //                    return "No value";
        //                else if (val != -2.0f) {
        //                    QString s;
        //                    QTextStream txt(&s);
        //                    txt.setRealNumberNotation(QTextStream::FixedNotation);
        //                    txt << val;
        //                    return s;
        //                } else
        //                    return "";
        //            }
        //        }
        //    }
    }

    return QObject::eventFilter(object, e);
}

void AGLMapViewport::zoomBy(float dzf, int mouseX, int mouseY) {
    GLfloat screenRatio = GLfloat(width()) / height();
    float pzf = m_zoomFactor;
    m_zoomFactor = m_zoomFactor * dzf;
    if (m_zoomFactor < m_minZoomFactor)
        m_zoomFactor = m_minZoomFactor;
    else if (m_zoomFactor > m_maxZoomFactor)
        m_zoomFactor = m_maxZoomFactor;
    m_eyePosX +=
        (m_zoomFactor - pzf) * screenRatio * GLfloat(mouseX - width() * 0.5f) / GLfloat(width());
    m_eyePosY -= (m_zoomFactor - pzf) * GLfloat(mouseY - height() * 0.5f) / GLfloat(height());
    update();
}

void AGLMapViewport::panBy(int dx, int dy) {
    m_eyePosX += m_zoomFactor * GLfloat(dx) / height();
    m_eyePosY -= m_zoomFactor * GLfloat(dy) / height();

    update();
}

Point2f AGLMapViewport::getWorldPoint(const QPoint &screenPoint) {
    return Point2f(+m_zoomFactor * float(screenPoint.x() - width() * 0.5) / height() - m_eyePosX,
                   -m_zoomFactor * float(screenPoint.y() - height() * 0.5) / height() - m_eyePosY);
}

QPoint AGLMapViewport::getScreenPoint(const Point2f &worldPoint) {
    return QPoint((worldPoint.x + m_eyePosX) * height() / m_zoomFactor + width() * 0.5,
                  -(worldPoint.y + m_eyePosY) * height() / m_zoomFactor + height() * 0.5);
}

void AGLMapViewport::resetView() {
    //    m_visiblePointMap.showLinks(false);
    //    m_visibleShapeGraph.showLinks(false);
    //    m_pDoc.getMetaGraph().clearSel();
    update();
}

void AGLMapViewport::setModeJoin() {
    //    if (m_pDoc.getMetaGraph().getViewClass() & (MetaGraph::VIEWVGA |
    //    MetaGraph::VIEWAXIAL)) {
    //        resetView();
    //        m_interactionMode = InteractionMode::JOIN;
    //        m_visiblePointMap.showLinks(true);
    //        m_visibleShapeGraph.showLinks(true);
    //        m_pDoc.getMetaGraph().clearSel();
    //        notifyDatasetChanged();
    //    }
}

void AGLMapViewport::setModeUnjoin() {
    //    if (m_pDoc.getMetaGraph().getState() & (MetaGraph::VIEWVGA |
    //    MetaGraph::VIEWAXIAL)) {
    //        resetView();
    //        m_interactionMode = InteractionMode::UNJOIN;
    //        m_visiblePointMap.showLinks(true);
    //        m_visibleShapeGraph.showLinks(true);
    //        m_pDoc.getMetaGraph().clearSel();
    //        notifyDatasetChanged();
    //    }
}

void AGLMapViewport::setModeFill() {
    resetView();
    m_interactionMode = InteractionMode::FILL_FULL;
}

void AGLMapViewport::setModeSemiFill() {
    resetView();
    m_interactionMode = InteractionMode::FILL_SEMI;
}

void AGLMapViewport::setModeAugmentFill() {
    resetView();
    m_interactionMode = InteractionMode::FILL_AUGMENT;
}

void AGLMapViewport::setModeSingleFill() {
    resetView();
    m_interactionMode = InteractionMode::FILL_SINGLE;
}

void AGLMapViewport::setModeIsovist() {
    resetView();
    m_interactionMode = InteractionMode::SEED_ISOVIST;
}

void AGLMapViewport::setModeTargetedIsovist() {
    resetView();
    m_interactionMode = InteractionMode::SEED_TARGETED_ISOVIST;
}

void AGLMapViewport::setModeSeedAxial() {
    resetView();
    m_interactionMode = InteractionMode::SEED_AXIAL;
}

void AGLMapViewport::setModeStepDepth() {
    resetView();
    m_interactionMode = InteractionMode::POINT_STEP_DEPTH;
}

void AGLMapViewport::setModeDrawLine() {
    resetView();
    m_interactionMode = InteractionMode::DRAW_LINE;
}

void AGLMapViewport::setModeDrawPolygon() {
    resetView();
    m_interactionMode = InteractionMode::DRAW_POLYGON;
}

void AGLMapViewport::setModeSelect() {
    resetView();
    m_interactionMode = InteractionMode::SELECT;
}

void AGLMapViewport::postLoadFile() {
    matchViewToCurrentMetaGraph();
    //    setWindowTitle(m_pDoc.m_base_title);
}

void AGLMapViewport::zoomToSelection() {
    //    if (m_pDoc.m_meta_graph && m_pDoc.getMetaGraph().isSelected()) {
    //        OnViewZoomToRegion(m_pDoc.getMetaGraph().getSelBounds());
    //    }
}

void AGLMapViewport::matchViewToCurrentMetaGraph() {
    const QtRegion &region = m_graphViewModel->getBoundingBox();
    zoomToRegion(region);
    update();
}

void AGLMapViewport::zoomToRegion(const QtRegion region) {
    if ((region.top_right.x == 0 && region.bottom_left.x == 0) ||
        (region.top_right.y == 0 && region.bottom_left.y == 0))
        // region is unset, don't try to change the view to it
        return;
    m_eyePosX = -(region.top_right.x + region.bottom_left.x) * 0.5f;
    m_eyePosY = -(region.top_right.y + region.bottom_left.y) * 0.5f;
    if (region.width() > region.height()) {
        m_zoomFactor = region.top_right.x - region.bottom_left.x;
    } else {
        m_zoomFactor = region.top_right.y - region.bottom_left.y;
    }
    m_minZoomFactor = m_zoomFactor * 0.001;
    m_maxZoomFactor = m_zoomFactor * 10;
}

// void GLMapView::OnEditCopy() {
//    std::unique_ptr<QDepthmapView> tmp(new QDepthmapView(m_pDoc, m_settings));
//    Point2f topLeftWorld = getWorldPoint(QPoint(0, 0));
//    Point2f bottomRightWorld = getWorldPoint(QPoint(width(), height()));

//    tmp->setAttribute(Qt::WA_DontShowOnScreen);
//    tmp->show();
//    tmp->postLoadFile();
//    tmp->OnViewZoomToRegion(QtRegion(topLeftWorld, bottomRightWorld));
//    tmp->repaint();
//    tmp->OnEditCopy();
//    tmp->close();
//}

// void GLMapView::OnEditSave() {
//    std::unique_ptr<QDepthmapView> tmp(new QDepthmapView(m_pDoc, m_settings));
//    Point2f topLeftWorld = getWorldPoint(QPoint(0, 0));
//    Point2f bottomRightWorld = getWorldPoint(QPoint(width(), height()));

//    tmp->setAttribute(Qt::WA_DontShowOnScreen);
//    tmp->show();
//    tmp->postLoadFile();
//    tmp->OnViewZoomToRegion(QtRegion(topLeftWorld, bottomRightWorld));
//    tmp->OnEditSave();
//}
