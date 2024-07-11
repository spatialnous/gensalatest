// SPDX-FileCopyrightText: 2000-2010 University College London, Alasdair Turner
// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
// SPDX-FileCopyrightText: 2024 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "salalib/attributemap.h"

class AttributeMapCLI {

  protected:
    std::unique_ptr<AttributeMap> m_map;

  public:
    AttributeMapCLI(std::unique_ptr<AttributeMap> &&map) : m_map(std::move(map)) {}
    AttributeMapCLI &operator=(AttributeMapCLI &&other) {
        m_map = std::move(other.m_map);
        return *this;
    }
    virtual ~AttributeMapCLI() {}
    AttributeMapCLI() = delete;
    AttributeMapCLI(const AttributeMapCLI &other) = delete;
    AttributeMapCLI(AttributeMapCLI &&other) = default;

    virtual AttributeMap &getInternalMap() { return *m_map; }
    virtual const AttributeMap &getInternalMap() const { return *m_map; }

    const AttributeTable &getAttributeTable() const { return getInternalMap().getAttributeTable(); }
    AttributeTable &getAttributeTable() { return getInternalMap().getAttributeTable(); }
    AttributeTableHandle &getAttributeTableHandle() {
        return getInternalMap().getAttributeTableHandle();
    }
    const AttributeTableHandle &getAttributeTableHandle() const {
        return getInternalMap().getAttributeTableHandle();
    }
    LayerManagerImpl &getLayers() { return getInternalMap().getLayers(); }

    const QtRegion &getRegion() const { return m_map->getRegion(); }
};
