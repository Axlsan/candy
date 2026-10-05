#include "spriteLibrary.h"
#include "SDL3_Image/SDL_image.h"
#include "entity.h"
#include <cassert>

using namespace std;

const char* FALLBACK_PATH = "assets/sprites/dog.png";

static const SpriteDataEntry all_sprite_data[] = {
  {SPRITE_ID::Fallback, FALLBACK_PATH,8,8},
  {SPRITE_ID::black_1x1, "assets/sprites/1x1_black.png",0,0},
  {SPRITE_ID::Button_Basic, "assets/sprites/basic_button.png",0,0,3,3},
  {SPRITE_ID::Background, "assets/sprites/background.png"},
  {SPRITE_ID::BackgroundAlt, "assets/sprites/backgroundAlt.png"},
  {SPRITE_ID::Wall, "assets/sprites/wall.png"},
  {SPRITE_ID::Square, "assets/sprites/square.png"},
  {SPRITE_ID::Sphere, "assets/sprites/sphere.png"},
  {SPRITE_ID::Triangle, "assets/sprites/triangle.png"},
  {SPRITE_ID::Plus, "assets/sprites/plus.png"},
  };

Sprite* GetSprite(SPRITE_ID spriteID, Sprite* spriteBuffer){
  return &spriteBuffer[(int)spriteID];
}

Sprite* GetSpriteFromEntityID(ENTITY_ID id, Sprite* spriteBuffer){
  /*Sprite* sprite = &spriteBuffer[(int)sprite_id];
  if(sprite == nullptr || sprite->texture == nullptr){
    assert(sprite_id != SPRITE_ID::Fallback);
    return GetSprite(SPRITE_ID::Fallback, spriteBuffer);
  }
  return &spriteBuffer[(int)sprite_id];*/
  Sprite* sprite = nullptr;

  switch(id){
    case ENTITY_ID::NONE:
      sprite = nullptr;
      break;
    case ENTITY_ID::BACKGROUND:
    case ENTITY_ID::BACKGROUND_ALT:
      sprite = &spriteBuffer[(int)SPRITE_ID::Background];
      break;
    case ENTITY_ID::WALL:
      sprite = &spriteBuffer[(int)SPRITE_ID::Wall];
      break;
    case ENTITY_ID::SQUARE:
      sprite = &spriteBuffer[(int)SPRITE_ID::Square];
      break;
    case ENTITY_ID::SPHERE:
      sprite = &spriteBuffer[(int)SPRITE_ID::Sphere];
      break;
    case ENTITY_ID::TRIANGLE:
      sprite = &spriteBuffer[(int)SPRITE_ID::Triangle];
      break;
    case ENTITY_ID::PLUS:
      sprite = &spriteBuffer[(int)SPRITE_ID::Plus];
      break;
  }

  if(sprite == nullptr || sprite->texture == nullptr){
    return &spriteBuffer[(int)SPRITE_ID::Fallback];
  }
  
  return sprite;
}

namespace AssetManagement{
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer *renderer){
    for (SpriteDataEntry entry : all_sprite_data) {
      LoadSprite(spriteBuffer, entry, renderer);
    }
  }

  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer){
    SDL_Surface* surface = IMG_Load(entry.path); 
    if(surface == nullptr){
      surface = IMG_Load(FALLBACK_PATH);
    }
    assert(surface != nullptr);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    Sprite* sprite = &spriteBuffer[(int)entry.id];
    sprite->texture = texture;
    sprite->height = texture->h;
    sprite->width = texture->w;
    if(entry.pivot_x == NOT_SET || entry.pivot_y == NOT_SET){
      sprite->pivot_x = sprite->width / 2;
      sprite->pivot_y = sprite->height / 2;
    }
    else{
      sprite->pivot_x = entry.pivot_x;
      sprite->pivot_y = entry.pivot_y;
    }

    sprite->sprite_count_x = entry.tileset_cell_count_x;
    sprite->sprite_count_y = entry.tileset_cell_count_y;
    sprite->framerate = entry.framerate;
    
    SDL_DestroySurface(surface);
  }
}
