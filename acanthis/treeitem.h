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

#include "QVariant"

#include <memory>
#include <vector>

class TreeItem {
    int m_row;
    QWeakPointer<TreeItem> m_parentItem;
    QList<QSharedPointer<TreeItem>> m_children;

  protected:
    QString m_name;
    bool m_visible = true;
    bool m_editable = false;

  public:
    TreeItem(QString name) : m_name(name) {}
    const QSharedPointer<TreeItem> addChildItem(QSharedPointer<TreeItem> treeItem, int row) {
        m_children.push_back(treeItem);
        m_children.back()->setRow(row);
        return m_children.back();
    }

    const QSharedPointer<TreeItem> getParent() const { return m_parentItem; };
    const int getRow() const { return m_row; }
    const int nChildren() const { return m_children.size(); }
    const QString getName() const { return m_name; }
    const bool isVisible() const { return m_visible; }
    const bool isEditable() const { return m_editable; }
    const QSharedPointer<TreeItem> getChild(size_t idx) { return m_children.at(idx); }

    void setParentItem(QWeakPointer<TreeItem> parent) { m_parentItem = parent; }
    void setRow(int row) { m_row = row; }
    void setVisible(bool visible) { m_visible = visible; }
    void setEditable(bool editable) { m_editable = editable; }
};
