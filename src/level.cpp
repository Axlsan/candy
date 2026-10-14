#include "level.h"
#include "Parsers/json.hpp"
#include "arena.h"
#include "entity.h"
#include <cstdint>
#include <fstream>
#include <vector>



void InitializeLevel(Arena* arena_levels, Level* lvl, int w, int h){

  //  + for walls
  lvl->w = w + 2;
  lvl->h = h + 1;
  int wWalls = w + 2;
  int hWalls = h + 1;
  
  lvl->cells = ALLOC_ARRAY(arena_levels, uint8_t, wWalls * hWalls);

  // Walls & background
  for (int y = 0; y < hWalls; ++y) {
    for (int x = 0; x < wWalls; ++x) {
      bool isWall = x == 0 || x == wWalls - 1 || y == hWalls - 1;
      uint8_t cellID = isWall ? 3 : 1;
      int index = y * lvl->w + x;
      lvl->cells[index] = cellID;
    }
  }



  // Shapes
  lvl->shapeBuffer = ALLOC_ARRAY(arena_levels, Entity, w * h);
  lvl->entityCount = 0;

  
  
  for(int y = 0; y < h; y++){
    for(int x = 0; x < w; x++){
      Entity& shape = lvl->shapeBuffer[lvl->entityCount++];

      shape.id = (ENTITY_ID)((int)ENTITY_ID::SQUARE + std::rand() % 4);
      shape.xPos = x+1;
      shape.yPos = y;
      shape.xPrev = x+1;
      shape.yPrev = -1;
      shape.progress01 = 0.0f;
    }
  }

  
  /*int entity_index = 0;
  std::memset(entityBuffer, 0, sizeof(Entity) * GameData::ENTITY_COUNT);
  
  for (int x = 0; x < 6; x++) {
    for (int y = 0; y < 6; y++) {
      char id = level->map[x][y];
      switch(id){
        case 'p':
          entityBuffer[entity_index].x = x;
          entityBuffer[entity_index].y = y;
          entityBuffer[entity_index].type = ENTITY_TYPE::Player;
          entity_index++;
          break;
        case 'b':
          entityBuffer[entity_index].x = x;
          entityBuffer[entity_index].y = y;
          entityBuffer[entity_index].type = ENTITY_TYPE::Box;
          entity_index++;
          break;
      }
    }  
  }*/  
}



using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;
/*
void CreateLevel(Arena* arena, Level* lvl, const char* levelName){
  fstream stream(levelName);
  bool a = stream.is_open();


  
  auto jsonResult = nlohmann::json::parse(stream);
  vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();

  lvl->w = jsonResult["width"].get<int>();
  lvl->h = jsonResult["height"].get<int>();
  lvl->levelPath = levelName;
  size_t size_of_cells = sizeof(uint8_t) * lvl->w * lvl->h;

  lvl->cells = (uint8_t *)Memory::Allocate(arena, size_of_cells);

  for (int i = 0; i < lvl->w * lvl->h; i++) {
    lvl->cells[i] = dataField[i];
  }
  
}

Entity* GetNextAvailableEntitySlot(Level* lvl){
  for(int i = 0; i < lvl->EntityCount; i++){
    if(lvl->shapeBuffer[i].id == ENTITY_ID::NONE){
      return &lvl->shapeBuffer[i];
    }
  }
  return &lvl->shapeBuffer[lvl->EntityCount++];
}

void AddEntity(ENTITY_ID entityID, int x, int y, Level* lvl){
  Entity* entity = GetEntity(lvl, x, y);
  if(entity == nullptr){
    entity = GetNextAvailableEntitySlot(lvl);
  }

  entity->xPos = x;
  entity->yPos = y;
  entity->xPrev = x;
  entity->yPrev = y;
  entity->id = entityID;
  
}

void CreateEntities(Arena* arena, Level* lvl){
  Reset(arena);
  lvl->EntityCount = 0;
  fstream stream(lvl->levelPath);
  auto result = nlohmann::json::parse(stream);
  auto entityData = result["layers"][ENTITIES_INDEX]["data"].get<vector<uint8_t>>();

  lvl->shapeBuffer = ALLOC_ARRAY(arena, Entity, lvl->w * lvl->h);

  for(int i = 0; i < lvl->w * lvl->h; i++){
    unsigned char entityID = entityData[i];
    if(entityID != 0){
      int x = i % lvl->w;
      int y = i / lvl->w;
      AddEntity((ENTITY_ID)entityID, x, y, lvl);
    }
  }
  
}


*/

Entity* GetEntity(Level* lvl, int x, int y){
  for(int i = 0; i < lvl->entityCount; i++){
    if(lvl->shapeBuffer[i].xPos == x && lvl->shapeBuffer[i].yPos == y){
      if(
        lvl->shapeBuffer[i].id != ENTITY_ID::NONE &&
        lvl->shapeBuffer[i].id != ENTITY_ID::MATCHED &&
        lvl->shapeBuffer[i].xPos == x &&
        lvl->shapeBuffer[i].yPos == y ){
        return &lvl->shapeBuffer[i];
      }
    }
  }
  return nullptr;
}

uint8_t GetCellID(Level* lvl, int x, int y){
  return lvl->cells[y * lvl->w + x];
}
