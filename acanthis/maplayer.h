// Copyright (C) 2022, Petros Koutsolampros

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

#include "treeitem.h"

#include "agl/composite/aglmap.h"

#include "salalib/attributetable.h"

#include <QObject>
#include <QString>

#include <memory>

class MapLayer : public QObject, public TreeItem {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(bool visible MEMBER m_visible NOTIFY visibilityChanged)

  protected:
    AttributeTable &m_attributes;

  public:
    MapLayer(QString mapName, AttributeTable &attributes)
        : TreeItem(mapName), m_attributes(attributes){};

    bool isVisible() { return m_visible; }
    QString getName() { return m_name; }
    AttributeTable &getAttributes() { return m_attributes; }

    virtual std::unique_ptr<AGLMap> constructGLMap() = 0;

    virtual bool hasGraph() { return false; }

  signals:
    void nameChanged();
    void visibilityChanged();
};
