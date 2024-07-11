// SPDX-FileCopyrightText: 2000-2010 University College London, Alasdair Turner
// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
// SPDX-FileCopyrightText: 2024 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "shapegraphcli.h"

void ShapeGraphCLI::makeConnections(const KeyVertices &keyvertices) {
    getInternalMap().makeConnections(keyvertices);
    m_displayed_attribute = -1; // <- override if it's already showing
    auto conn_col = getInternalMap().getAttributeTable().getColumnIndex("Connectivity");

    setDisplayedAttribute(static_cast<int>(conn_col));
}

void ShapeGraphCLI::unlinkFromShapeMap(const ShapeMap &shapemap) {
    getInternalMap().unlinkFromShapeMap(shapemap);

    // reset displayed attribute if it happens to be "Connectivity":
    auto conn_col = getInternalMap().getAttributeTable().getColumnIndex("Connectivity");
    if (getDisplayedAttribute() == static_cast<int>(conn_col)) {
        invalidateDisplayedAttribute();
        setDisplayedAttribute(
            static_cast<int>(conn_col)); // <- reflect changes to connectivity counts
    }
}

void ShapeGraphCLI::makeSegmentConnections(std::vector<Connector> &connectionset) {
    getInternalMap().makeSegmentConnections(connectionset);

    m_displayed_attribute = -2; // <- override if it's already showing

    auto uw_conn_col = getInternalMap().getAttributeTable().getColumnIndex("Connectivity");
    setDisplayedAttribute(static_cast<int>(uw_conn_col));
}

bool ShapeGraphCLI::read(std::istream &stream) {

    bool read = getInternalMap().readShapeGraphData(stream);
    // now base class read:
    read = read && ShapeMapCLI::read(stream);

    return read;
}

bool ShapeGraphCLI::write(std::ostream &stream) {
    bool written = getInternalMap().writeShapeGraphData(stream);

    // now simply run base class write:
    written = written & ShapeMapCLI::write(stream);

    return written;
}
