#include "shapemaplayer.h"

#include "glmapview/glshapemap.h"

ShapeMapLayer::ShapeMapLayer(ShapeMap &shapeMap)
    : m_shapeMap(shapeMap), MapLayer(std::unique_ptr<GLShapeMap>(
                                new GLShapeMap(shapeMap, 8, shapeMap.getSpacing() * 0.1))) {
    m_name = QString::fromStdString(shapeMap.getName());
}
