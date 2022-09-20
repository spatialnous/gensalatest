#pragma once

#include "attributelayer.h"

#include <QObject>
#include <QString>
#include <memory>

class MapLayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)

  protected:
    QString m_name;
    std::vector<std::unique_ptr<AttributeLayer>> m_attributes;

  public:
    MapLayer(){};
    AttributeLayer &getAttribute(std::size_t index) {
        return *m_attributes[index];
    }

  signals:
    void nameChanged();
};
