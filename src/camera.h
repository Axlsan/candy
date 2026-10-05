#pragma once

#include "level.h"
struct Camera{
  float camera_x;
  float camera_y;
};
namespace camera {
  void GridToWorld(float* x, float* y, const Level* lvl);
  void WorldToGrid(float worldX, float worldY, int* x, int* y, const Level* lvl);
  bool IsPointInGrid(float x, float y, const Level* lvl);
};
