#pragma once

#include "graphdocument.h"
#include "maplayer.h"
#include "attributelayer.h"

#include <QAbstractListModel>

class AttributeModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(
        GraphDocument *graphDocument MEMBER m_graphDocument NOTIFY graphDocumentChanged)

    GraphDocument *m_graphDocument;

  public:
    enum AttributeModelRoles {
        AttributeRole = Qt::UserRole,
    };

    explicit AttributeModel(QObject *parent = nullptr);

    //    LayeredImageProject *layeredImageProject() const;
    //    void setLayeredImageProject(LayeredImageProject *layeredImageProject);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    void graphDocumentChanged();

  public slots:
    void onPreAttributesCleared();
    void onPostAttributesCleared();
    void onPreAttributeAdded(int index);
    void onPostAttributeAdded(int index);
    void onPreAttributeRemoved(int index);
    void onPostAttributeRemoved(int index);
    void onPreAttributeMoved(int fromIndex, int toIndex);
    void onPostAttributeMoved(int fromIndex, int toIndex);

    //  private:
    //    LayeredImageProject *mLayeredImageProject;
};
