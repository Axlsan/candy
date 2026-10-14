#pragma once

#include "entity.h"
#include "arena.h"
#include <cstdint>
#include <cstdlib>
#include <vector>

using namespace Memory;
using namespace std;

struct Level{

  
  int w;
  int h;

  // vector<vector<int>> map = {};
  
  
  uint8_t* cells;
  const char* levelPath;
  Entity* shapeBuffer;
  int entityCount;

  int score;
};

uint8_t GetCellID(Level* lvlData, int x, int y);
Entity* GetEntity(Level* lvl, int x, int y);

Entity* GetNextAvailableEntitySlot(Level* lvl);
void AddEntity(ENTITY_ID entityID, int x, int y, Level lvl);

void CreateLevel(Arena* arena, Level* lvl, const char* levelName);
void CreateEntities(Arena* arena, Level* lvl);

uint8_t* GetPlayableCells(Level* lvl);

void InitializeLevel(Arena* arena_levels, Level* lvl, int w, int h);
