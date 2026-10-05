#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "arena.h"
#include "audioSystem.h"
#include "button.h"
#include "common.h"
#include "dev_gui.h"
#include "entity.h"
#include "fontLibrary.h"
#include "gameState.h"
#include "imgui/imgui.h"
#include "input.h"
#include "level.h"
#include "levelRenderer.h"
#include "spriteLibrary.h"
#include <cstddef>
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
  
  void InitializeGame(Gameplay* gameplay, Arena* arena_levels){
    assert(gameplay->initialized == false);

    gameplay->currentLvlIndex = 0;
    CreateLevel(arena_levels, &gameplay->lvls[0], "assets/levels/lvl1.tmj");
    gameplay->initialized = true;
  }

  void UpdateTitleScreen(TitleScreen* titleScreen, const float dt){
    
  }

  void UpdateGame(Gameplay* gameplay, Input* input, const float){
    
  }

  void StartLevel(Gameplay* gameplay, Arena* arenaCommands, Arena* arenaEntities){
    Reset(arenaCommands);
    CreateEntities(arenaEntities, &gameplay->lvls[gameplay->currentLvlIndex]);
  }

  void ChangeScene(GameData* data, SCENE_TYPES newScene){
    assert(newScene != data->sceneCurrent);

    data->scenePrevious = data->sceneCurrent;
    data->sceneCurrent = newScene;
    data->transition.state = data->scenePrevious == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    data->transition.fadeTimeElapsed = 0;

    switch (data->sceneCurrent) {

    case SCENE_TYPES::NONE:
      assert(false);
    case SCENE_TYPES::TITLESCREEN:
      data->transition.fadeTimeDuration = 1.0f;
      break;
    case SCENE_TYPES::MAINMENU:
      break;
    case SCENE_TYPES::GAME:{
      data->transition.fadeTimeDuration = 2.5f;
      Gameplay* gameplay = &data->scenes.gameplay;
      assert(gameplay->initialized);
      StartLevel(gameplay, data->arena_commands, data->arena_entities);
      break;
    }
    case SCENE_TYPES::CREDITS:
      break;
    }
  }
  
  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    SetUpBoilerplate(data, window, renderer);

    // data->currentLevelIndex = 0;
    // CreateLevel(data->arena_levels, &data->levels[0], "assets/levels/lvl1.tmj");
        
    InitializeGame(&data->scenes.gameplay, data->arena_levels);
    ChangeScene(data, SCENE_TYPES::GAME);

    data->entity.id = ENTITY_ID::SQUARE;
    data->entity.xPos = 400;
    data->entity.yPos = 200;

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

  
  void Update(GameData* data,float dt){
    EditorData* editorData = &data->editor_data;

    Gameplay* gameplay = &data->scenes.gameplay;
    TitleScreen* titleScreen = &data->scenes.titleScreen;
    Transition* transition = &data->transition;

    if(transition->state != Transition::Inactive){
      transition->fadeTimeElapsed += dt;
      if(transition->fadeTimeElapsed >= transition->fadeTimeDuration){
        switch (transition->state) {
          case Transition::Inactive:
            break;
          case Transition::FadeTo:
            transition->state = Transition::FadeFrom;
            break;
          case Transition::FadeFrom:
            transition->state = Transition::Inactive;
            break;
        }
      }
    }

    switch (data->sceneCurrent) {
      case SCENE_TYPES::TITLESCREEN:
        UpdateTitleScreen(titleScreen, dt);
        if(AnyKeyPressed(&data->input)){
          if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
            ChangeScene(data, SCENE_TYPES::GAME);
          }
        }
        break;

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:
        UpdateGame(gameplay, &data->input, dt);
        break;
      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
    }

    
    Level* currentLvl = &data->scenes.gameplay.lvls[data->scenes.gameplay.currentLvlIndex];
    // 3 in a row
    for(int x = 0; x < currentLvl->w; x++){
      // ENTITY_ID a = currentLvl->shapeBuffer
      
    }

    
    
  } 

  
  void DrawScene(GameData* data, SCENE_TYPES scene, SDL_Renderer* renderer){
    switch (scene) {
      case SCENE_TYPES::TITLESCREEN:{
        // Sprite* background = GetSprite(SPRITE_ID::titleScreenBackground, data->spriteBuffer);
        break;
      }

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        break;

      case SCENE_TYPES::CREDITS:
        break;

      case SCENE_TYPES::NONE:
        assert(false);
    }
  }
  
  void Draw(GameData* data, SDL_Renderer* renderer){
    DEV::PreDraw(data->imGui_context);
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
    SDL_RenderClear(renderer);

    
    RenderLevel(data, renderer);
    RenderEntities(data, renderer);
    
    SDL_Texture* groundTex = GetSprite(SPRITE_ID::Background, data->spriteBuffer)->texture;
    SDL_Texture* groundAltTex = GetSprite(SPRITE_ID::BackgroundAlt, data->spriteBuffer)->texture;
    SDL_Texture* wallTex = GetSprite(SPRITE_ID::Wall, data->spriteBuffer)->texture;
    SDL_Texture* squareTex = GetSprite(SPRITE_ID::Square, data->spriteBuffer)->texture;
    SDL_Texture* sphereTex = GetSprite(SPRITE_ID::Sphere, data->spriteBuffer)->texture;
    SDL_Texture* triangleTex = GetSprite(SPRITE_ID::Triangle, data->spriteBuffer)->texture;
    SDL_Texture* plusTex = GetSprite(SPRITE_ID::Plus, data->spriteBuffer)->texture;
    SDL_FRect squareRect = {(float)data->entity.xPos, (float)data->entity.yPos, 64, 64};

    SDL_RenderTexture(renderer, squareTex, NULL, &squareRect);
    // SDL_RenderTextureRotated(renderer, squareTex, NULL, squareRect, 0, NULL, SDL_FLIP_NONE);

    switch(data->transition.state){
      case Transition::Inactive:
        DrawScene(data, data->sceneCurrent, renderer);
        break;
      
      case Transition::FadeTo:{
        DrawScene(data, data->scenePrevious, renderer);
        float alpha = data->transition.fadeTimeElapsed / data->transition.fadeTimeDuration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }

      case Transition::FadeFrom:{
        DrawScene(data, data->sceneCurrent, renderer);
        float alpha = 1 - data->transition.fadeTimeElapsed / data->transition.fadeTimeDuration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }
    }
    
    
    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }

}
