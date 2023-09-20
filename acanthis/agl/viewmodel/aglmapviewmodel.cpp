#include "aglmapviewmodel.h"


const QList<QSharedPointer<MapLayer>> &AGLMapViewModel::getMaps() const {
    return m_graphViewModel->getMapLayers();
}

AGLMap &AGLMapViewModel::getGLMap(MapLayer *mapLayer) {
    auto glMap = m_glMaps.find(mapLayer);
    if (glMap == m_glMaps.end()) {
        // AGLMap has not been created, create it.
        auto newGLMap = m_glMaps.insert(std::make_pair(mapLayer, mapLayer->constructGLMap()));

        return *(newGLMap.first)->second;
    }
    return *glMap->second;
}

void AGLMapViewModel::cleanup() {
    for (auto &glMap : m_glMaps) {
        glMap.second->cleanup();
    }
}

void AGLMapViewModel::loadGLObjects() {
    for (auto &map : getMaps()) {
        getGLMap(map.get()).loadGLObjects();
    }
}

void AGLMapViewModel::initializeGL(bool m_core) {
    for (auto &map : getMaps()) {
        getGLMap(map.get()).initializeGL(m_core);
    }
}

void AGLMapViewModel::loadGLObjectsRequiringGLContext() {
    for (auto &map : getMaps()) {
        getGLMap(map.get()).loadGLObjectsRequiringGLContext();
    }
}

void AGLMapViewModel::highlightHoveredItems(const QtRegion &region) {
    for (auto &map : getMaps()) {
        getGLMap(map.get()).highlightHoveredItems(region);
    }
}

void AGLMapViewModel::updateGL(bool m_core) {
    for (auto &map : getMaps()) {
        if (!map->isVisible())
            continue;
        getGLMap(map.get()).updateGL(m_core);
        getGLMap(map.get()).updateHoverGL(m_core);
    }
}

void AGLMapViewModel::paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                           const QMatrix4x4 &m_mModel) {
    for (auto &map : getMaps()) {
        if (!map->isVisible())
            continue;
        getGLMap(map.get()).paintGL(m_mProj, m_mView, m_mModel);
    }
}
