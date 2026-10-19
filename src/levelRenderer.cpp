#include "levelRenderer.h"
#include "arena.h"
#include "entity.h"
#include "gameState.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <algorithm>
#include <cmath>

bool IsEntityBelowOtherEntity(Entity *a, Entity *b) { return a->yPos < b->yPos; };

void RenderLevel(GameData* data, SDL_Renderer* renderer){
  Level lvlData = data->scenes.gameplay.lvls[data->scenes.gameplay.currentLvlIndex];

  for(int x = 0; x < lvlData.w; x++){
    for(int y = 0; y < lvlData.h; y++){
      uint8_t cellType = GetCellID(&lvlData, x, y);
      Sprite* sprite;

      if(ENTITY_ID(cellType) == ENTITY_ID::BACKGROUND){
        sprite = &data->spriteBuffer[(x + y) % 2 == 0 ? (int)SPRITE_ID::Background : (int)SPRITE_ID::BackgroundAlt];
      }
      else{
        sprite = GetSpriteFromEntityID((ENTITY_ID)cellType, data->spriteBuffer);
      }

      if(sprite == nullptr){
        continue;
      }
      RenderSprite_Grid(sprite, &lvlData, renderer, &data->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  Level* lvl = &data->scenes.gameplay.lvls[data->scenes.gameplay.currentLvlIndex];

  Entity** SortedEntities = ALLOC_ARRAY(data->arena_scratch, Entity*, lvl->entityCount);
  for(int i = 0; i < lvl->entityCount; i++){
    SortedEntities[i] = &lvl->shapeBuffer[i];
  }

  std::sort(SortedEntities, SortedEntities + lvl->entityCount, IsEntityBelowOtherEntity);

  for(int i = 0; i < lvl->entityCount; i++){
    Entity* entity = SortedEntities[i];
    if(entity->id == ENTITY_ID::NONE){
      continue;
    }

    Sprite* sprite = GetSpriteFromEntityID(entity->id, data->spriteBuffer);

    // Animation
    float xAnimated = std::lerp(entity->xPrev, entity->xPos, entity->progress01 < 0.5 ? 4 * entity->progress01 * entity->progress01 * entity->progress01 : 1 - std::pow(-2 * entity->progress01 + 2, 3) / 2);
    float yAnimated = std::lerp(entity->yPrev, entity->yPos, entity->progress01 < 0.5 ? 4 * entity->progress01 * entity->progress01 * entity->progress01 : 1 - std::pow(-2 * entity->progress01 + 2, 3) / 2);

    // // Animation
    // float xAnimated = std::lerp(entity->xPrev, entity->xPos, std::pow(entity->progress01, 3));
    // float yAnimated = std::lerp(entity->yPrev, entity->yPos, std::pow(entity->progress01, 3));

    float scaleAnimated = entity->scaleUp ? 1.0 + sin(entity->scaleTimer) / 3 : 1.0;

// RenderEntity_OnTile(sprite, lvl, renderer, &data->camera, xAnimated, yAnimated, 1, 1)
    RenderSprite_Grid(sprite, lvl, renderer, &data->camera, xAnimated, yAnimated, scaleAnimated, 1, false, entity->scaleUp);
  }
}
