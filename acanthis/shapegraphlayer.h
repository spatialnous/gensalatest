#pragma once

#include "maplayer.h"

#include "salalib/axialmap.h"

class ShapeGraphLayer : public MapLayer {
    ShapeGraph &m_shapeMap;

  public:
    explicit ShapeGraphLayer(ShapeGraph &shapeGraph);
};
