#pragma once

#include <QObject>
#include <QString>

class MapLayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)

  protected:
    QString m_name;

  public:
    MapLayer(){};

  signals:
    void nameChanged();
};
