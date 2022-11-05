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

#include "salalib/attributetable.h"

#include <QObject>
#include <QString>

class AttributeItem : public QObject, public TreeItem {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(bool visible MEMBER m_visible NOTIFY visibilityChanged)

    AttributeColumn &m_attributeColumn;

  public:
    AttributeItem(AttributeColumn &attributeColumn)
        : TreeItem(QString::fromStdString(attributeColumn.getName())),
          m_attributeColumn(attributeColumn) {
        m_visible = false;
    };

  signals:
    void nameChanged();
    void visibilityChanged();
};
