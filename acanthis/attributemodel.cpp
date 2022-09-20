#include "attributemodel.h"

#include <QString>

AttributeModel::AttributeModel(QObject *parent)
    : QAbstractListModel(parent), m_graphDocument(nullptr) {}

// void AttributeModel::setGraphDocument(GraphDocument *layeredImageProject) {
//    if (layeredImageProject == mLayeredImageProject)
//        return;

//    if (mLayeredImageProject) {
//        mLayeredImageProject->disconnect(this);
//    }

//    beginResetModel();
//    mLayeredImageProject = layeredImageProject;
//    endResetModel();
//    emit layeredImageProjectChanged();

//    if (mLayeredImageProject) {
//        connect(mLayeredImageProject, &LayeredImageProject::preLayersCleared, this,
//        &AttributeModel::onPreLayersCleared); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayersCleared, this, &AttributeModel::onPostLayersCleared);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerAdded, this,
//        &AttributeModel::onPreLayerAdded); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerAdded, this, &AttributeModel::onPostLayerAdded);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this,
//        &AttributeModel::onPreLayerRemoved); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerRemoved, this, &AttributeModel::onPostLayerRemoved);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerMoved, this,
//        &AttributeModel::onPreLayerMoved); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerMoved, this, &AttributeModel::onPostLayerMoved);
//    }
//}

QVariant AttributeModel::data(const QModelIndex &index, int role) const {
    if (!m_graphDocument || !index.isValid())
        return QVariant();

    AttributeLayer *layer = m_graphDocument->attributeAt(0, index.row());
    if (!layer)
        return QVariant();

    if (role == AttributeRole) {
        return QVariant::fromValue(layer);
    }
    return QVariant();
}

int AttributeModel::rowCount(const QModelIndex &) const {
    if (!m_graphDocument)
        return 0;

    return m_graphDocument->layerCount();
}

int AttributeModel::columnCount(const QModelIndex &) const { return 1; }

QHash<int, QByteArray> AttributeModel::roleNames() const {
    QHash<int, QByteArray> names;
    names.insert(AttributeRole, "layer");
    return names;
}

void AttributeModel::onPreAttributesCleared() {
    //    qCDebug(lcAttributeModel) << "about to call beginResetModel()";
    beginResetModel();
    //    qCDebug(lcAttributeModel) << "called beginResetModel()";
}

void AttributeModel::onPostAttributesCleared() {
    //    qCDebug(lcAttributeModel) << "about to call endResetModel()";
    endResetModel();
    //    qCDebug(lcAttributeModel) << "called endResetModel()";
}

void AttributeModel::onPreAttributeAdded(int index) {
    //    qCDebug(lcAttributeModel) << "index" << index;
    beginInsertRows(QModelIndex(), index, index);
}

void AttributeModel::onPostAttributeAdded(int index) {
    //    qCDebug(lcAttributeModel) << "index" << index;
    endInsertRows();
}

void AttributeModel::onPreAttributeRemoved(int index) {
    //    qCDebug(lcAttributeModel) << "index" << index;
    beginRemoveRows(QModelIndex(), index, index);
}

void AttributeModel::onPostAttributeRemoved(int index) {
    //    qCDebug(lcAttributeModel) << "index" << index;
    endRemoveRows();
}

void AttributeModel::onPreAttributeMoved(int fromIndex, int toIndex) {
    // Ahhh... what a mess. The behaviour of beginMoveRows() is super confusing
    // when moving an item down within the same parent, so we account for that weirdness
    // here. http://doc.qt.io/qt-5/qabstractitemmodel.html#beginMoveRows
    const int actualToIndex = toIndex > fromIndex ? toIndex + 1 : toIndex;

    //    if (!beginMoveRows(QModelIndex(), fromIndex, fromIndex, QModelIndex(),
    //    actualToIndex)) {
    //        qWarning() << "beginMoveRows() failed when trying to move" << fromIndex <<
    //        "to" << toIndex
    //                   << "- weird stuff might be about to happen since we're not going
    //                   to cancel the move operation";
    //    }
}

void AttributeModel::onPostAttributeMoved(int, int) { endMoveRows(); }
