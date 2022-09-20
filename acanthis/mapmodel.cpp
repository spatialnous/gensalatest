#include "mapmodel.h"

#include <QString>

MapModel::MapModel(QObject *parent)
    : QAbstractListModel(parent), m_graphDocument(nullptr) {}

// void MapModel::setGraphDocument(GraphDocument *layeredImageProject) {
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
//        &MapModel::onPreLayersCleared); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayersCleared, this, &MapModel::onPostLayersCleared);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerAdded, this,
//        &MapModel::onPreLayerAdded); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerAdded, this, &MapModel::onPostLayerAdded);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this,
//        &MapModel::onPreLayerRemoved); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerRemoved, this, &MapModel::onPostLayerRemoved);
//        connect(mLayeredImageProject, &LayeredImageProject::preLayerMoved, this,
//        &MapModel::onPreLayerMoved); connect(mLayeredImageProject,
//        &LayeredImageProject::postLayerMoved, this, &MapModel::onPostLayerMoved);
//    }
//}

QVariant MapModel::data(const QModelIndex &index, int role) const {
    if (!m_graphDocument || !index.isValid())
        return QVariant();

    MapLayer *layer = m_graphDocument->layerAt(index.row());
    if (!layer)
        return QVariant();

    if (role == LayerRole) {
        return QVariant::fromValue(layer);
    }
    return QVariant();
}

int MapModel::rowCount(const QModelIndex &) const {
    if (!m_graphDocument)
        return 0;

    return m_graphDocument->layerCount();
}

int MapModel::columnCount(const QModelIndex &) const { return 1; }

QHash<int, QByteArray> MapModel::roleNames() const {
    QHash<int, QByteArray> names;
    names.insert(LayerRole, "layer");
    return names;
}

void MapModel::onPreLayersCleared() {
    //    qCDebug(lcMapModel) << "about to call beginResetModel()";
    beginResetModel();
    //    qCDebug(lcMapModel) << "called beginResetModel()";
}

void MapModel::onPostLayersCleared() {
    //    qCDebug(lcMapModel) << "about to call endResetModel()";
    endResetModel();
    //    qCDebug(lcMapModel) << "called endResetModel()";
}

void MapModel::onPreLayerAdded(int index) {
    //    qCDebug(lcMapModel) << "index" << index;
    beginInsertRows(QModelIndex(), index, index);
}

void MapModel::onPostLayerAdded(int index) {
    //    qCDebug(lcMapModel) << "index" << index;
    endInsertRows();
}

void MapModel::onPreLayerRemoved(int index) {
    //    qCDebug(lcMapModel) << "index" << index;
    beginRemoveRows(QModelIndex(), index, index);
}

void MapModel::onPostLayerRemoved(int index) {
    //    qCDebug(lcMapModel) << "index" << index;
    endRemoveRows();
}

void MapModel::onPreLayerMoved(int fromIndex, int toIndex) {
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

void MapModel::onPostLayerMoved(int, int) { endMoveRows(); }
