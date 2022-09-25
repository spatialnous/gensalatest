#pragma once

#include <QAbstractListModel>
#include <graphdocument.h>

class MapModel : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(GraphDocument *graphDocument MEMBER m_graphDocument NOTIFY graphDocumentChanged)

    GraphDocument *m_graphDocument;
    QSharedPointer<TreeItem> m_rootItem;
    TreeItem *getItem(const QModelIndex &idx) const;

  public:
    enum LayerModelRoles {
        LayerRole = Qt::UserRole,
    };

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

  signals:
    void graphDocumentChanged();

  public slots:
    void onPreLayersCleared();
    void onPostLayersCleared();
    void onPreLayerAdded(int index);
    void onPostLayerAdded(int index);
    void onPreLayerRemoved(int index);
    void onPostLayerRemoved(int index);
    void onPreLayerMoved(int fromIndex, int toIndex);
    void onPostLayerMoved(int fromIndex, int toIndex);
};
