#pragma once

#include "QVariant"
#include <memory>
#include <vector>

class TreeItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(bool visible MEMBER m_visible NOTIFY visibilityChanged)

  public:
    QString m_name;
    bool m_visible = true;

    int row;
    QSharedPointer<TreeItem> parentItem;
    QList<QSharedPointer<TreeItem>> subItems;

  signals:
    void nameChanged();
    void visibilityChanged();
};
