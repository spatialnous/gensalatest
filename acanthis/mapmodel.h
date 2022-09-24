#pragma once

#include <QAbstractListModel>
#include <graphdocument.h>

class MapModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(GraphDocument *graphDocument MEMBER m_graphDocument NOTIFY graphDocumentChanged)

    GraphDocument *m_graphDocument;

  public:
    enum LayerModelRoles {
        LayerRole = Qt::UserRole,
    };

    explicit MapModel(QObject *parent = nullptr);

    //    LayeredImageProject *layeredImageProject() const;
    //    void setLayeredImageProject(LayeredImageProject *layeredImageProject);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

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

    //  private:
    //    LayeredImageProject *mLayeredImageProject;
};
