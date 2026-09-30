#pragma once

#include "entity.h"
#include <cstdint>
struct Pipe{
  float x;
  float gap_y;
  static constexpr float GAP_SIZE = 350;
  static constexpr float PIPE_BASE_SPEED = 500;
};

struct Level{
  static const int PIPE_COUNT = 3;
  Pipe pipes[PIPE_COUNT];  
};


struct LevelData{
  int w;
  int h;
  uint8_t* cells;
  const char* levelPath;
  Entity* entityBuffer;
  int entityCount;
  
  uint8_t GetCellID(int x, int y){
    return cells[y * w + x];
  }
};


void AddEntity(EntityID entityID, int x, int y, LevelData* lvl);
