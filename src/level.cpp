#include "level.h"
#include "Parsers/json.hpp"
#include "arena.h"
#include "entity.h"
#include <fstream>



using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;

void CreateLevel(Arena* arena, Level* lvl, const char* levelName){
  fstream stream(levelName);
  bool a = stream.is_open();

  // stream.close();
  // stream.open(levelName);
  
  // auto cwd = std::filesystem::current_path();
  // auto resolvedPath = std::filesystem::absolute(levelName);

  // std::printf("Working directory: %s\n", cwd.string().c_str());
  // std::printf("Resolved path: %s\n", resolvedPath.string().c_str());
  // std::printf("File exists: %s\n",
  //                         std::filesystem::exists(resolvedPath) ? "yes" : "no");

  
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
  // CreateEntities(arena, lvl);
  
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


uint8_t GetCellID(Level* lvl, int x, int y){
  return lvl->cells[y * lvl->w + x];
}

Entity* GetEntity(Level* lvl, int x, int y){
  for(int i = 0; i < lvl->EntityCount; i++){
    if(lvl->shapeBuffer[i].xPos == x && lvl->shapeBuffer[i].yPos == y){
      return &lvl->shapeBuffer[i];
    }
  }
  return nullptr;
}
