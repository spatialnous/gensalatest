#pragma once

#include <QObject>
#include <QString>

class AttributeLayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)

  protected:
    QString m_name;

  public:
    AttributeLayer(){};

  signals:
    void nameChanged();
};
