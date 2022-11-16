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

#include <graphdocument.h>

#include <QAbstractListModel>
#include <qqml.h>

class MapModel : public QAbstractItemModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(GraphDocument *graphDocument MEMBER m_graphDocument NOTIFY graphDocumentChanged)

    GraphDocument *m_graphDocument;
    QSharedPointer<TreeItem> m_rootItem;
    TreeItem *getItem(const QModelIndex &idx) const;

    enum LayerModelRoles { NameRole = Qt::UserRole, VisibilityRole };

    QSharedPointer<TreeItem> addChildItem(QSharedPointer<TreeItem> parent,
                                          QSharedPointer<TreeItem> newChild, int row);
    QSharedPointer<TreeItem> addChildItem(QSharedPointer<TreeItem> parent, QString newChild,
                                          int row);

  public:
    explicit MapModel(QObject *parent = nullptr);

    void updateAfterGraphDocumentChange();

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void resetItems();
    Q_INVOKABLE void setItemVisibility(const QModelIndex &idx, bool visibility);

  signals:
    void graphDocumentChanged();
};
