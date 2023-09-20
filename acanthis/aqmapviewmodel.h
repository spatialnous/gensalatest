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

#include "graphviewmodel.h"

#include <QAbstractListModel>
#include <qqml.h>

class AQMapViewModel : public QAbstractItemModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GraphViewModel *graphViewModel
               MEMBER m_graphViewModel
               NOTIFY graphViewModelChanged)

    GraphViewModel *m_graphViewModel;
    QSharedPointer<TreeItem> m_rootItem;
    TreeItem *getItem(const QModelIndex &idx) const;

    enum LayerModelRole { NameRole = Qt::UserRole, VisibleRole, EditableRole };

    QSharedPointer<TreeItem> addChildItem(QSharedPointer<TreeItem> parent,
                                          QSharedPointer<TreeItem> newChild, int row);
    QSharedPointer<TreeItem> addChildItem(QSharedPointer<TreeItem> parent, QString newChild,
                                          int row);

  public:
    explicit AQMapViewModel(QObject *parent = nullptr);

    void updateAfterGraphViewModelChange();

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    AQMapViewModel::LayerModelRole getRole(int columnIndex) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void resetItems();
    Q_INVOKABLE void setItemVisible(const QModelIndex &idx, bool visibility);
    Q_INVOKABLE void setItemEditable(const QModelIndex &idx, bool visibility);

  signals:
    void graphViewModelChanged();
};
