#include "pixelmaplayer.h"

PixelMapLayer::PixelMapLayer(PointMap &pointMap) : m_pointMap(pointMap) {
    m_name = QString::fromStdString(m_pointMap.getName());
}
