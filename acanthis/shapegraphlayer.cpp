#include "shapegraphlayer.h"

#include "glmapview/glshapegraph.h"

ShapeGraphLayer::ShapeGraphLayer(ShapeGraph &shapeMap)
    : m_shapeMap(shapeMap), MapLayer(std::unique_ptr<GLShapeGraph>(
                                new GLShapeGraph(shapeMap, 8, shapeMap.getSpacing() * 0.1))) {
    m_name = QString::fromStdString(shapeMap.getName());
}
