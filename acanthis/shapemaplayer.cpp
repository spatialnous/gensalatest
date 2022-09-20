#include "shapemaplayer.h"

ShapeMapLayer::ShapeMapLayer(ShapeMap &shapeMap) : m_shapeMap(shapeMap) {
    m_name = QString::fromStdString(shapeMap.getName());
}
