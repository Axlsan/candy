#pragma once



#include "SDL3/SDL_render.h"
#include "gameState.h"

void RenderLevel(GameData* data, SDL_Renderer* renderer);
void RenderEntities(GameData* data, SDL_Renderer* renderer);

