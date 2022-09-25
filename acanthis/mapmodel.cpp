#include "mapmodel.h"

#include <QString>

MapModel::MapModel(QObject *parent) : QAbstractItemModel(parent), m_rootItem(new TreeItem) {

    // make sure that the whole model is reset when the graph document changes
    connect(this, &MapModel::graphDocumentChanged, [=]() { updateAfterGraphDocumentChange(); });
}

void MapModel::updateAfterGraphDocumentChange() {
    beginResetModel();
    //    m_rootItem = new TreeItem({"root1"});

    //    for (std::unique_ptr<MapLayer> &mapLayer : m_graphDocument->getMapLayers())
    //        m_rootItem->appendChild(new TreeItem({mapLayer->getName()}, m_rootItem));
    endResetModel();
}

QModelIndex MapModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    TreeItem *parentItem = getItem(parent);
    auto childPtr = parentItem->subItems.at(row);
    if (childPtr) {
        return createIndex(row, column, childPtr.get());
    } else {
        return QModelIndex();
    }
}

QModelIndex MapModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }
    TreeItem *childItem = getItem(index);
    auto parentPtr = childItem->parentItem;
    if (!parentPtr || parentPtr == m_rootItem) {
        return QModelIndex();
    }
    return createIndex(parentPtr.get()->row, 0, parentPtr.get());
}

int MapModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem = getItem(parent);
    return parentItem->subItems.size();
}

int MapModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 1;
}

QVariant MapModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    TreeItem *item = getItem(index);
    // MapLayer *layer = m_graphDocument->layerAt(index.row());
    switch (role) {
    case LayerRole:
        return QVariant::fromValue(item);
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> MapModel::roleNames() const {
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names.insert(QHash<int, QByteArray>{{LayerRole, "treeitem"}});
    return names;
}

void MapModel::resetItems() {
    beginResetModel();
    for (std::unique_ptr<MapLayer> &mapLayer : m_graphDocument->getMapLayers()) {

        //        QSharedPointer<TreeItem> mappl{new TreeItem};
        mapLayer->parentItem = m_rootItem;
        m_rootItem->subItems.append(QSharedPointer<TreeItem>(mapLayer.get()));
        //        mapLayer->m_name = QString(mapLayer->getName()).arg(0);
        //        mappl->name = QString(mapLayer->getName()).arg(0);
    }
    endResetModel();
}

TreeItem *MapModel::getItem(const QModelIndex &idx) const {
    if (idx.isValid()) {
        TreeItem *item = static_cast<TreeItem *>(idx.internalPointer());
        if (item) {
            return item;
        }
    }
    return m_rootItem.get();
}

//// void MapModel::setGraphDocument(GraphDocument *layeredImageProject) {
////    if (layeredImageProject == mLayeredImageProject)
////        return;

////    if (mLayeredImageProject) {
////        mLayeredImageProject->disconnect(this);
////    }

////    beginResetModel();
////    mLayeredImageProject = layeredImageProject;
////    endResetModel();
////    emit layeredImageProjectChanged();

////    if (mLayeredImageProject) {
////        connect(mLayeredImageProject, &LayeredImageProject::preLayersCleared, this,
////        &MapModel::onPreLayersCleared); connect(mLayeredImageProject,
////        &LayeredImageProject::postLayersCleared, this, &MapModel::onPostLayersCleared);
////        connect(mLayeredImageProject, &LayeredImageProject::preLayerAdded, this,
////        &MapModel::onPreLayerAdded); connect(mLayeredImageProject,
////        &LayeredImageProject::postLayerAdded, this, &MapModel::onPostLayerAdded);
////        connect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this,
////        &MapModel::onPreLayerRemoved); connect(mLayeredImageProject,
////        &LayeredImageProject::postLayerRemoved, this, &MapModel::onPostLayerRemoved);
////        connect(mLayeredImageProject, &LayeredImageProject::preLayerMoved, this,
////        &MapModel::onPreLayerMoved); connect(mLayeredImageProject,
////        &LayeredImageProject::postLayerMoved, this, &MapModel::onPostLayerMoved);
////    }
////}

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
