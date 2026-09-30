#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "audioSystem.h"
#include "button.h"
#include "common.h"
#include "dev_gui.h"
#include "fontLibrary.h"
#include "gameState.h"
#include "imgui/imgui.h"
#include "input.h"
#include "level.h"
#include "spriteLibrary.h"
#include <cmath>
using namespace Memory;

extern "C" {

  void SetUpBoilerplate(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    DEV::Initialize(window, renderer);
    InitializeAudioSystem(&data->audio, data->arena_main);
    AssetManagement::LoadAllSFX(&data->audio);
    AssetManagement::LoadAllSprites(data->spriteBuffer, renderer);
    data->imGui_context = ImGui::GetCurrentContext();
    AssetManagement::LoadFont(renderer, "assets/fonts/ByteBounce.ttf", &data->font, 48);

    SDL_Texture* blackfade = GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);
    // PlaySong(SONG_ID::THEME); // just muted for my sanity
  }
  
  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    SetUpBoilerplate(data, window, renderer);
    
    SDL_Texture* wallTex     = GetSprite(SPRITE_ID::Wall, data->spriteBuffer)->texture;
    SDL_Texture* squareTex   = GetSprite(SPRITE_ID::Square, data->spriteBuffer)->texture;
    SDL_Texture* sphereTex   = GetSprite(SPRITE_ID::Sphere, data->spriteBuffer)->texture;
    SDL_Texture* triangleTex = GetSprite(SPRITE_ID::Triangle, data->spriteBuffer)->texture;
    SDL_Texture* plusTex     = GetSprite(SPRITE_ID::Plus, data->spriteBuffer)->texture;

    // SDL_SetTextureScaleMode(appleTex, SDL_SCALEMODE_PIXELART);
    // SDL_SetTextureScaleMode(snakeTex, SDL_SCALEMODE_PIXELART);

    // flappy bird setup

    data->shape->x = 400;
    data->shape->y = SCREEN_HEIGHT / 2.0;
    data->shape->y -= 150;
    data->gravity = 10; // fake gravity

    data->grace_timer = 1;

    data->level.pipes[0].x = 800;
    data->level.pipes[0].gap_y = SCREEN_HEIGHT / 2.0;
    data->level.pipes[1].x = 1800;
    data->level.pipes[1].gap_y = SCREEN_HEIGHT / 2.0;
    data->level.pipes[2].x = 2300;
    data->level.pipes[2].gap_y = SCREEN_HEIGHT / 2.0;

    data->game_over_timer = 2;
  }
   
   bool HandleEvents(GameData *data, SDL_Event event){
    DEV::ProcessEvents(&event);

    if(event.type != SDL_EVENT_KEY_DOWN){
        return true;
    }
    if(event.key.key == SDLK_ESCAPE){
        return false;
    }

     return true;
  }

// helper function
  void ResetGame(GameData* data){
    data->shape->x = 400;
    data->shape->y = SCREEN_HEIGHT / 2.0;
    data->shape->y -= 150;

    data->game_over_timer = 2;
    data->score = 0;
    data->gameState = GAME_STATE::GAME;
    data->grace_timer = 1;
    
    Pipe* pipe = &data->level.pipes[0];
    pipe->x = 800;

    for (int i = 0; i < Level::PIPE_COUNT; i++) {
      Pipe* pipe = &data->level.pipes[i];
      pipe->x = (i + 1) * 800;
    }
    
  }
  bool PipeCollisionCheck(Bird* bird, Pipe* pipe){
    if(bird->x + 64 < pipe->x){
      return false;
    }

    if(bird->x > pipe->x + 64){
      return false;
    }

    float birdY = bird->y;

    if(birdY < pipe->gap_y){
      return true;
    }

    if(birdY + 64 > pipe->gap_y + Pipe::GAP_SIZE){
      return true;
    }

    return false;
  } 
  void Update(GameData* data, float dt){
    EditorData* editorData = &data->editor_data;

    if(KeyPressed(&data->input, SDL_SCANCODE_R)){
      ResetGame(data);
    }
    
  }

  // helper
  void DrawPipe(Pipe* pipe, SDL_Renderer* renderer, SDL_Texture* texture){
    float pipeHeight = 700;
    SDL_FRect pipeRect_top    = {pipe->x, pipe->gap_y - pipeHeight, 150, pipeHeight};
    SDL_FRect pipeRect_bottom = {pipe->x, pipe->gap_y + pipe->GAP_SIZE ,150 ,pipeHeight};
    SDL_RenderTextureRotated(renderer, texture, NULL, &pipeRect_top, NULL, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderTextureRotated(renderer, texture, NULL, &pipeRect_bottom, NULL, NULL, SDL_FLIP_VERTICAL);
  }
      
  void Draw(GameData* data, SDL_Renderer* renderer){
    DEV::PreDraw(data->imGui_context);
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
    SDL_RenderClear(renderer);

    // render bird
    Entity shape = *data->shape;
    // Bird bird = data->bird;
    SDL_Texture* birdTex = GetSprite(SPRITE_ID::Square, data->spriteBuffer)->texture;
    SDL_FRect bird_rect = {(float)shape.x, (float)shape.y, 64, 64};

    // render pipes
    SDL_Texture* pipeTex = GetSprite(SPRITE_ID::Fallback, data->spriteBuffer)->texture;
    for (int i = 0; i < Level::PIPE_COUNT; i++) {
        DrawPipe(&data->level.pipes[i], renderer, pipeTex);
    }
        
    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }

}
