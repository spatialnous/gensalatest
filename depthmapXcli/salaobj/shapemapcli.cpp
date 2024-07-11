// SPDX-FileCopyrightText: 2000-2010 University College London, Alasdair Turner
// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
// SPDX-FileCopyrightText: 2024 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "shapemapcli.h"
#include "salalib/tolerances.h"
#include <numeric>

void ShapeMapCLI::init(size_t size, const QtRegion &r) {
    m_display_shapes.clear();
    getInternalMap().init(size, r);
}

double ShapeMapCLI::getDisplayMinValue() const {
    return (m_displayed_attribute != -1)
               ? getInternalMap().getDisplayMinValue(static_cast<size_t>(m_displayed_attribute))
               : 0;
}

double ShapeMapCLI::getDisplayMaxValue() const {
    return (m_displayed_attribute != -1)
               ? getInternalMap().getDisplayMaxValue(static_cast<size_t>(m_displayed_attribute))
               : getInternalMap().getDefaultMaxValue();
}

const DisplayParams &ShapeMapCLI::getDisplayParams() const {
    return getInternalMap().getDisplayParams(static_cast<size_t>(m_displayed_attribute));
}

void ShapeMapCLI::setDisplayParams(const DisplayParams &dp, bool applyToAll) {
    getInternalMap().setDisplayParams(dp, static_cast<size_t>(m_displayed_attribute), applyToAll);
}

void ShapeMapCLI::setDisplayedAttribute(int col) {
    if (!m_invalidate && m_displayed_attribute == col) {
        return;
    }
    m_displayed_attribute = col;
    m_invalidate = true;

    // always override at this stage:
    getInternalMap().getAttributeTableHandle().setDisplayColIndex(m_displayed_attribute);

    m_invalidate = false;
}

void ShapeMapCLI::setDisplayedAttribute(const std::string &col) {
    setDisplayedAttribute(
        static_cast<int>(getInternalMap().getAttributeTable().getColumnIndex(col)));
}

int ShapeMapCLI::getDisplayedAttribute() const {
    if (m_displayed_attribute == getInternalMap().getAttributeTableHandle().getDisplayColIndex())
        return m_displayed_attribute;
    if (getInternalMap().getAttributeTableHandle().getDisplayColIndex() != -2) {
        m_displayed_attribute = getInternalMap().getAttributeTableHandle().getDisplayColIndex();
    }
    return m_displayed_attribute;
}

float ShapeMapCLI::getDisplayedAverage() {
    return (static_cast<float>(
        getInternalMap().getDisplayedAverage(static_cast<size_t>(m_displayed_attribute))));
}

void ShapeMapCLI::invalidateDisplayedAttribute() { m_invalidate = true; }

void ShapeMapCLI::clearAll() {
    m_display_shapes.clear();
    getInternalMap().clearAll();
    m_displayed_attribute = -1;
}

int ShapeMapCLI::makePointShape(const Point2f &point, bool tempshape,
                                const std::map<int, float> &extraAttributes) {
    return makePointShapeWithRef(point, getInternalMap().getNextShapeKey(), tempshape,
                                 extraAttributes);
}

bool ShapeMapCLI::read(std::istream &stream) {

    m_display_shapes.clear();

    bool read = false;
    std::tie(read, m_editable, m_show, m_displayed_attribute) = getInternalMap().read(stream);

    invalidateDisplayedAttribute();
    setDisplayedAttribute(m_displayed_attribute);

    return true;
}

bool ShapeMapCLI::write(std::ostream &stream) {
    bool written = getInternalMap().writeNameType(stream);

    stream.write((char *)&m_show, sizeof(m_show));
    stream.write((char *)&m_editable, sizeof(m_editable));

    written = written && getInternalMap().writePart2(stream);

    // TODO: Compatibility. The attribute columns will be stored sorted
    // alphabetically so the displayed attribute needs to match that
    auto sortedDisplayedAttribute = getInternalMap().getAttributeTable().getColumnSortedIndex(
        static_cast<size_t>(m_displayed_attribute));
    stream.write((char *)&sortedDisplayedAttribute, sizeof(sortedDisplayedAttribute));
    written = written && getInternalMap().writePart3(stream);
    return written;
}

bool ShapeMapCLI::findNextShape(bool &nextlayer) const {
    // note: will not work immediately after a new poly has been added:
    // makeViewportShapes first
    if (m_newshape) {
        return false;
    }

    // TODO: Remove static_cast<size_t>(-1)
    while ((++m_current_shape < (int)getInternalMap().getAllShapes().size()) &&
           m_display_shapes[static_cast<size_t>(m_current_shape)] == static_cast<size_t>(-1))
        ;

    if (m_current_shape < (int)getInternalMap().getAllShapes().size()) {
        return true;
    } else {
        m_current_shape = (int)getInternalMap().getAllShapes().size();
        nextlayer = true;
        return false;
    }
}

const SalaShape &ShapeMapCLI::getNextShape() const {
    auto x = m_display_shapes[static_cast<size_t>(m_current_shape)]; // x has display order in it
    m_display_shapes[static_cast<size_t>(m_current_shape)] =
        static_cast<size_t>(-1); // you've drawn it
    return depthmapX::getMapAtIndex(getInternalMap().getAllShapes(), x)->second;
}

// this is all very similar to spacepixel, apart from a few minor details

void ShapeMapCLI::makeViewportShapes(const QtRegion &viewport) const {

    auto &shapes = getInternalMap().getAllShapes();
    if (m_display_shapes.empty() || m_newshape) {
        m_display_shapes.assign(shapes.size(), static_cast<size_t>(-1));
        m_newshape = false;
    }

    m_current_shape = -1; // note: findNext expects first to be labelled -1

    m_display_shapes = getInternalMap().makeViewportShapes(viewport);

    m_curlinkline = -1;
    m_curunlinkpoint = -1;
}

int ShapeMapCLI::makePointShapeWithRef(const Point2f &point, int shape_ref, bool tempshape,
                                       const std::map<int, float> &extraAttributes) {
    int shapeRef =
        getInternalMap().makePointShapeWithRef(point, shape_ref, tempshape, extraAttributes);
    if (!tempshape) {
        m_newshape = true;
    }
    return shapeRef;
}

int ShapeMapCLI::makeLineShape(const Line &line, bool through_ui, bool tempshape,
                               const std::map<int, float> &extraAttributes) {
    return makeLineShapeWithRef(line, getInternalMap().getNextShapeKey(), through_ui, tempshape,
                                extraAttributes);
}

int ShapeMapCLI::makeLineShapeWithRef(const Line &line, int shape_ref, bool through_ui,
                                      bool tempshape, const std::map<int, float> &extraAttributes) {
    // note, map must have editable flag on if we are to make a shape through the
    // user interface:
    if (through_ui && !m_editable) {
        return -1;
    }
    int shapeRef = getInternalMap().makeLineShapeWithRef(line, shape_ref, through_ui, tempshape,
                                                         extraAttributes);

    if (!tempshape) {
        m_newshape = true;
    }

    if (through_ui) {
        // update displayed attribute if through ui:
        invalidateDisplayedAttribute();
        setDisplayedAttribute(m_displayed_attribute);
    }
    return shapeRef;
}

int ShapeMapCLI::makePolyShape(const std::vector<Point2f> &points, bool open, bool tempshape,
                               const std::map<int, float> &extraAttributes) {
    return makePolyShapeWithRef(points, getInternalMap().getNextShapeKey(), open, tempshape,
                                extraAttributes);
}

int ShapeMapCLI::makePolyShapeWithRef(const std::vector<Point2f> &points, bool open, int shape_ref,
                                      bool tempshape, const std::map<int, float> &extraAttributes) {
    int shapeRef =
        getInternalMap().makePolyShapeWithRef(points, open, shape_ref, tempshape, extraAttributes);
    if (!tempshape) {
        m_newshape = true;
    }
    return shapeRef;
}

int ShapeMapCLI::makeShape(const SalaShape &poly, int override_shape_ref,
                           const std::map<int, float> &extraAttributes) {
    int shapeRef = getInternalMap().makeShape(poly, override_shape_ref, extraAttributes);
    m_newshape = true;
    return shapeRef;
}

// n.b., only works from current selection (and uses point selected attribute)

int ShapeMapCLI::makeShapeFromPointSet(const PointMapCLI &pointmap) {
    int shapeRef =
        getInternalMap().makeShapeFromPointSet(pointmap.getInternalMap(), pointmap.getSelSet());
    m_newshape = true;
    return shapeRef;
}

bool ShapeMapCLI::moveShape(int shaperef, const Line &line, bool undoing) {
    bool moved = getInternalMap().moveShape(shaperef, line, undoing);

    if (getInternalMap().hasGraph()) {
        // update displayed attribute for any changes:
        invalidateDisplayedAttribute();
        setDisplayedAttribute(m_displayed_attribute);
    }
    return moved;
}

int ShapeMapCLI::polyBegin(const Line &line) {
    auto newShapeRef = getInternalMap().polyBegin(line);

    // update displayed attribute
    invalidateDisplayedAttribute();
    setDisplayedAttribute(m_displayed_attribute);

    // flag new shape
    m_newshape = true;

    return newShapeRef;
}

bool ShapeMapCLI::polyAppend(int shape_ref, const Point2f &point) {
    return getInternalMap().polyAppend(shape_ref, point);
}
bool ShapeMapCLI::polyClose(int shape_ref) { return getInternalMap().polyClose(shape_ref); }

bool ShapeMapCLI::polyCancel(int shape_ref) {
    bool polyCancelled = getInternalMap().polyCancel(shape_ref);

    // update displayed attribute
    invalidateDisplayedAttribute();
    setDisplayedAttribute(m_displayed_attribute);
    return polyCancelled;
}

bool ShapeMapCLI::removeSelected() {
    // note, map must have editable flag on if we are to remove shape:
    if (!m_editable) {
        return false;
        // m_selectionSet selection set is in order!
        // (it should be: code currently uses add() throughout)
        for (auto &shapeRef : m_selectionSet) {
            removeShape(shapeRef, false);
        }
        m_selectionSet.clear();

        invalidateDisplayedAttribute();
        setDisplayedAttribute(m_displayed_attribute);

        return true;
    }
    return false;
}

void ShapeMapCLI::removeShape(int shaperef, bool undoing) {
    getInternalMap().removeShape(shaperef, undoing);

    m_invalidate = true;
    m_newshape = true;
}

void ShapeMapCLI::undo() {
    getInternalMap().undo();
    invalidateDisplayedAttribute();
    setDisplayedAttribute(m_displayed_attribute);
    m_newshape = true;
}

void ShapeMapCLI::makeShapeConnections() {
    getInternalMap().makeShapeConnections();

    m_displayed_attribute = -1; // <- override if it's already showing
    auto conn_col = getInternalMap().getAttributeTable().getColumnIndex("Connectivity");

    setDisplayedAttribute(static_cast<int>(conn_col));
}

double ShapeMapCLI::getLocationValue(const Point2f &point) const {
    return getInternalMap().getLocationValue(point, m_displayed_attribute);
}

const PafColor ShapeMapCLI::getShapeColor() const {
    AttributeKey key(static_cast<int>(m_display_shapes[static_cast<size_t>(m_current_shape)]));
    const AttributeRow &row = getInternalMap().getAttributeTable().getRow(key);
    return dXreimpl::getDisplayColor(key, row, getInternalMap().getAttributeTableHandle(),
                                     m_selectionSet, true);
    ;
}

bool ShapeMapCLI::getShapeSelected() const {
    return m_selectionSet.find(static_cast<int>(
               m_display_shapes[static_cast<size_t>(m_current_shape)])) != m_selectionSet.end();
}

bool ShapeMapCLI::linkShapes(const Point2f &p) {
    if (m_selectionSet.size() != 1) {
        return false;
    }
    return getInternalMap().linkShapes(p, *m_selectionSet.begin());
}

bool ShapeMapCLI::unlinkShapes(const Point2f &p) {
    if (m_selectionSet.size() != 1) {
        return false;
    }
    clearSel();
    return getInternalMap().unlinkShapes(p, *m_selectionSet.begin());
}

bool ShapeMapCLI::findNextLinkLine() const {
    if (m_curlinkline < (int)getInternalMap().getLinks().size()) {
        m_curlinkline++;
    }
    return (m_curlinkline < (int)getInternalMap().getLinks().size());
}

Line ShapeMapCLI::getNextLinkLine() const {
    // note, links are stored directly by rowid, not by key:
    if (m_curlinkline < (int)getInternalMap().getLinks().size()) {
        return Line(depthmapX::getMapAtIndex(
                        getInternalMap().getAllShapes(),
                        getInternalMap().getLinks()[static_cast<size_t>(m_curlinkline)].a)
                        ->second.getCentroid(),
                    depthmapX::getMapAtIndex(
                        getInternalMap().getAllShapes(),
                        getInternalMap().getLinks()[static_cast<size_t>(m_curlinkline)].b)
                        ->second.getCentroid());
    }
    return Line();
}
bool ShapeMapCLI::findNextUnlinkPoint() const {
    if (m_curunlinkpoint < (int)getInternalMap().getUnlinks().size()) {
        m_curunlinkpoint++;
    }
    return (m_curunlinkpoint < (int)getInternalMap().getUnlinks().size());
}

Point2f ShapeMapCLI::getNextUnlinkPoint() const {
    // note, links are stored directly by rowid, not by key:
    if (m_curunlinkpoint < (int)getInternalMap().getUnlinks().size()) {
        return intersection_point(
            depthmapX::getMapAtIndex(
                getInternalMap().getAllShapes(),
                getInternalMap().getUnlinks()[static_cast<size_t>(m_curunlinkpoint)].a)
                ->second.getLine(),
            depthmapX::getMapAtIndex(
                getInternalMap().getAllShapes(),
                getInternalMap().getUnlinks()[static_cast<size_t>(m_curunlinkpoint)].b)
                ->second.getLine(),
            TOLERANCE_A);
    }
    return Point2f();
}

void ShapeMapCLI::getPolygonDisplay(bool &show_lines, bool &show_fill, bool &show_centroids) {
    show_lines = m_showLines;
    show_fill = m_showFill;
    show_centroids = m_showCentroids;
}

void ShapeMapCLI::setPolygonDisplay(bool show_lines, bool show_fill, bool show_centroids) {
    m_showLines = show_lines;
    m_showFill = show_fill;
    m_showCentroids = show_centroids;
}

std::vector<std::pair<SimpleLine, PafColor>>
ShapeMapCLI::getAllLinesWithColour(const std::set<int> &selSet) {
    return getInternalMap().getAllSimpleLinesWithColour(selSet);
}

std::vector<std::pair<std::vector<Point2f>, PafColor>>
ShapeMapCLI::getAllPolygonsWithColour(const std::set<int> &selSet) {
    return getInternalMap().getAllPolygonsWithColour(selSet);
}

std::vector<std::pair<Point2f, PafColor>>
ShapeMapCLI::getAllPointsWithColour(const std::set<int> &selSet) {
    return getInternalMap().getAllPointsWithColour(selSet);
}

std::vector<Point2f> ShapeMapCLI::getAllUnlinkPoints() {
    return getInternalMap().getAllUnlinkPoints();
}

bool ShapeMapCLI::setCurSel(const std::vector<int> &selset, bool add) {
    if (add == false) {
        clearSel();
    }

    for (auto shape : selset) {
        m_selectionSet.insert(shape);
    }

    return !m_selectionSet.empty();
}

bool ShapeMapCLI::setCurSel(QtRegion &r, bool add) {
    if (add == false) {
        clearSel();
    }

    std::map<int, SalaShape> shapesInRegion = getInternalMap().getShapesInRegion(r);
    for (auto &shape : shapesInRegion) {
        m_selectionSet.insert(shape.first);
    }

    return !shapesInRegion.empty();
}

float ShapeMapCLI::getSelectedAvg(size_t attributeIdx) {
    return getInternalMap().getAttributeTable().getSelAvg(attributeIdx, m_selectionSet);
}

bool ShapeMapCLI::clearSel() {
    // note, only clear if need be, as m_attributes->deselectAll is slow
    if (m_selectionSet.size()) {
        m_selectionSet.clear();
    }
    return true;
}

QtRegion ShapeMapCLI::getSelBounds() {
    QtRegion r;
    if (m_selectionSet.size()) {
        for (auto &shapeRef : m_selectionSet) {
            r = runion(r, getInternalMap().getAllShapes().at(shapeRef).getBoundingBox());
        }
    }
    return r;
}

bool ShapeMapCLI::selectionToLayer(const std::string &name) {
    bool retvar = false;
    if (m_selectionSet.size()) {
        dXreimpl::pushSelectionToLayer(getInternalMap().getAttributeTable(),
                                       getInternalMap().getLayers(), name, m_selectionSet);
        retvar = getInternalMap().getLayers().isLayerVisible(
            getInternalMap().getLayers().getLayerIndex(name));
        if (retvar) {
            clearSel();
        }
    }
    return retvar;
}
