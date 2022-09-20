#pragma once

#include "maplayer.h"

#include "salalib/shapemap.h"

class ShapeMapLayer : public MapLayer {
    ShapeMap &m_shapeMap;

  public:
    explicit ShapeMapLayer(ShapeMap &shapeMap);
};
