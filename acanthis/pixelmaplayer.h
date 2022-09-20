#pragma once

#include "maplayer.h"
#include "salalib/pointdata.h"

class PixelMapLayer : public MapLayer {

    PointMap &m_pointMap;

  public:
    explicit PixelMapLayer(PointMap &pixelMap);
};
