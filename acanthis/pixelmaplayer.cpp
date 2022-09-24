#include "pixelmaplayer.h"
#include "glmapview/glpixelmap.h"

PixelMapLayer::PixelMapLayer(PointMap &pointMap)
    : m_pointMap(pointMap), MapLayer(std::unique_ptr<GLPixelMap>(new GLPixelMap(pointMap))) {
    m_name = QString::fromStdString(m_pointMap.getName());
}
