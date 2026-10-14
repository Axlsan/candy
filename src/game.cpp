#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_Image/SDL_image.h"
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
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <vector>
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
  
  void InitializeTitleScreen(GameData* data){
    
    data->scenes.titleScreen.oceanTex = GetSprite(SPRITE_ID::OceanTitle, data->spriteBuffer)->texture;
    SDL_SetTextureScaleMode(data->scenes.titleScreen.oceanTex, SDL_SCALEMODE_PIXELART);

    data->scenes.titleScreen.logoTex = GetSprite(SPRITE_ID::Logo, data->spriteBuffer)->texture;
    SDL_SetTextureScaleMode(data->scenes.titleScreen.logoTex, SDL_SCALEMODE_PIXELART);
    SDL_SetTextureBlendMode(data->scenes.titleScreen.logoTex, SDL_BLENDMODE_BLEND);

    data->scenes.titleScreen.oceanRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 1.33};

    float logoScale = 0.5;
    float logoW = data->scenes.titleScreen.logoTex->w * logoScale;
    float logoH = data->scenes.titleScreen.logoTex->h * logoScale;

    data->scenes.titleScreen.logoRect = {
      (SCREEN_WIDTH / 2) - (logoW / 2),
      (SCREEN_HEIGHT / 3) - (logoW / 2),
      logoW,
      logoH};

    data->scenes.titleScreen.oceanProgress = 0.0;
    data->scenes.titleScreen.logoProgress = 0.0;
  }
  
  void InitializeGame(Gameplay* gameplay, Arena* arena_levels){
    assert(gameplay->initialized == false);

    
    gameplay->initialized = true;
  }

  void UpdateTitleScreen(TitleScreen* titleScreen, Input* input, const float dt){
    // BACKGROUND ANIMATION
    // length
    titleScreen->oceanProgress += dt / 5.0;
    if(titleScreen->oceanProgress > 1.0){
      titleScreen->oceanProgress = 1.0;
    }
    // easeinout
    float eased = 1 - (1 - titleScreen->oceanProgress) * (1 - titleScreen->oceanProgress);
    titleScreen->oceanRect.y = -((SCREEN_HEIGHT*1.3-SCREEN_HEIGHT)/2) * eased;

    // LOGO ANIMATION
    // length
    titleScreen->logoProgress += dt / 3.0;
    if(titleScreen->logoProgress > 1.0){
      titleScreen->logoProgress = 1.0;
    }
    // easeinout
    float logoEased = 1 - (1 - titleScreen->logoProgress) * (1 - titleScreen->logoProgress);
    titleScreen->logoRect.y = -((SCREEN_HEIGHT*1.3-SCREEN_HEIGHT)/2) * logoEased;

    SDL_SetTextureAlphaModFloat(titleScreen->logoTex, eased);
    

    if(KeyPressed(input, SDL_SCANCODE_RETURN)){
      // ChangeScene(data, SCENE_TYPES::GAME);
    }
  }

  
  bool TryFall(Entity* mover, Level* lvl){

    int testX = mover->xPos;
    int testY = mover->yPos + 1;
    
    Entity* fallIntoEntity = GetEntity(lvl, testX, testY);
    Entity* aboveEntity = GetEntity(lvl, mover->xPos, mover->yPos-1);
    uint8_t fallIntoTileID = GetCellID(lvl, testX, testY);

    if(fallIntoEntity == nullptr){
      if(fallIntoTileID == 3){
        mover->xPos = testX;
        mover->yPos = testY;
        mover->progress01 = 0;
        TryFall(aboveEntity, lvl);
        return true;
      }
      return false;
    }
    
    return false;
  }


  void WaitForSwapAni(Gameplay* gameplay, Level* lvl, float dt){
    float duration = 1.0;
    bool animating;
    for(int i = 0; i < lvl->entityCount; i++){
      Entity& entity = lvl->shapeBuffer[i];
      if(entity.id == ENTITY_ID::NONE){
        continue;
      }


      if(entity.progress01 < 1.0){
        entity.progress01 += dt / duration;

        if(entity.progress01 > 1.0){
          entity.progress01 = 1.0;
        }
        else{
          animating = true;
        }
      }

      if(animating){
        gameplay->animationInProgress = true;
      }
    }
    
  }
  
  bool EntityClick(Gameplay* gameplay, Level* lvl, Input* input){
  
    // printf("CLICLK");
    if(camera::IsPointInGrid(input->mouse_x, input->mouse_y, &gameplay->lvls[gameplay->currentLvlIndex])){
      int x = -1;
      int y = -1;
      camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, &gameplay->lvls[gameplay->currentLvlIndex]);

      
      if(GetCellID(&gameplay->lvls[gameplay->currentLvlIndex], (float)x, (float)y) == 1){
        if(gameplay->firstSelection != nullptr){
          gameplay->secondSelection = GetEntity(lvl, x, y);
          printf("First : ID: %hhu, X: %d , Y: %d\n", gameplay->firstSelection->id, gameplay->firstSelection->xPos, gameplay->firstSelection->yPos);
          printf("Second: ID: %hhu, X: %d , Y: %d\n", gameplay->secondSelection->id, gameplay->secondSelection->xPos, gameplay->secondSelection->yPos);

          
          Entity* first = gameplay->firstSelection;
          Entity* second = gameplay->secondSelection;

          // if same entity is clicked
          if(first == second){
            gameplay->firstSelection = nullptr;
            gameplay->secondSelection = nullptr;
            return false;
          }

          // if not neighbour
          if(
            !(abs(first->xPos - second->xPos) == 1 && first->yPos == second->yPos ||
            first->xPos == second->xPos && abs(first->yPos - second->yPos) == 1
            )){

            printf("NO NEIGHBOUR!\n");
            gameplay->firstSelection = nullptr;
            gameplay->secondSelection = nullptr;
            return false;
          }

          gameplay->animationInProgress = true;
          
          Entity tempEntity = *gameplay->firstSelection;
          
          // SWAPPING
          // set previous
          first->xPrev = first->xPos;
          first->yPrev = first->yPos;
          second->xPrev = second->xPos;
          second->yPrev = second->yPos;
          // set current
          first->xPos = second->xPos;
          first->yPos = second->yPos;
          second->xPos = tempEntity.xPos;
          second->yPos = tempEntity.yPos;
          // set progress
          first->progress01 = 0;
          second->progress01 = 0;
          
          gameplay->firstSelection = nullptr;
          gameplay->secondSelection = nullptr;
          
          return true;
        }
        else{
          gameplay->firstSelection = GetEntity(lvl, x, y);
          printf("First : ID: %hhu, X: %d , Y: %d\n", gameplay->firstSelection->id, gameplay->firstSelection->xPos, gameplay->firstSelection->yPos);
          printf("Second: ID: -, X: - , Y: -\n");
          return false;
        }

      }
      else{
        gameplay->firstSelection = nullptr;
      }
    }
    else{
      gameplay->firstSelection = nullptr;
      printf("First : ID: -, X: - , Y: -\n");
      printf("Second: ID: -, X: - , Y: -\n");
      return false;
    }
    return false;
  }


  bool InBounds(Level* lvl, int x, int y){
    return x >= 0 && y >= 0 && x < lvl->w && y < lvl->h;
  }

  bool ContainsEntity(std::vector<Entity*>& list, Entity* entity){
    for(Entity* e : list){
      if(e == entity){
        return true;
      }
    }
    return false;
  }
  

  int ComboCheck(Level* lvl, ENTITY_ID id, int x, int y, int xDir, int yDir, std::vector<Entity*>& matchedEntities){
    // check is in bounds
    if(!InBounds(lvl, x, y)){
      return 0;
    }
    // check if match
    Entity* entity = GetEntity(lvl, x, y);
    if(entity == nullptr || entity->id != id){
      return 0;
    }
    // check if already matched
    if(!ContainsEntity(matchedEntities, entity)){
      matchedEntities.push_back(entity);
    }
    
    // go to next
    int newX = x + xDir;
    int newY = y + yDir;
    return 1 + ComboCheck(lvl, id, newX, newY, xDir, yDir, matchedEntities);
  }
  
  // All ENTITY_ID::Matched are removed
  int RemoveMatches(Level* lvl, std::vector<Entity*> matchedEntities){
    int removed = 0;
    for(Entity* entity : matchedEntities){
      if(entity == nullptr) continue;
      entity->id = ENTITY_ID::NONE;
      entity->xPos = -1;
      entity->yPos = -1;
      removed++;
    }
    return removed;
  }

  void FallDown(Level* lvl){
    // every column
    for(int x = 0; x < lvl->w; x++){
      // above teh bottom wall
      int writeY = lvl->h - 2;
      for(int y = lvl->h - 2; y >= 0; y--){
        Entity* entity = GetEntity(lvl, x, y);
        if(entity == nullptr){
          continue;
        }
        // animate fall down
        entity->xPrev = entity->xPos;
        entity->xPos = x;
        entity->yPrev = entity->yPos;
        entity->yPos = writeY;
        entity->progress01 = 0.0;
        writeY--;
      }
    }
  }

  void SpawnFromTop(Level* lvl){
    for(int x = 1; x < lvl->w - 1; x++){
      for(int y = 0; y < lvl->h - 1; y++){
        if(GetEntity(lvl, x, y) != nullptr){
          continue;
        }

        Entity* newEntity;
        // find free cell
        for(int i = 0; i < lvl->entityCount; i++){
          if(lvl->shapeBuffer[i].id == ENTITY_ID::NONE){
            newEntity = &lvl->shapeBuffer[i];
            break;
          }
        }

        if(newEntity == nullptr){
          continue;
        }

        newEntity->id = (ENTITY_ID)((int)ENTITY_ID::SQUARE + rand() % 4);
        newEntity->xPrev = x;
        newEntity->yPrev = -2;
        newEntity->xPos = x;
        newEntity->yPos = y;
        newEntity->progress01 = 0.0;
      }
    }
  }

  bool ScanBoard(Level* lvl){
    int maxMatches = lvl->w * lvl->h;
    std::vector<Entity*> matchedEntities;
    std::vector<Entity*> hMatches;
    std::vector<Entity*> vMatches;
    // Entity** matchedEntities = new Entity*[maxMatches];
    for(int y = 0; y < lvl->h; y++){
      for(int x = 0; x < lvl->w; x++){
        Entity* entity = GetEntity(lvl, x, y);
        if(entity == nullptr){
          continue;
        }

        // horizontal search
        hMatches.clear();
        int hCount = ComboCheck(lvl, entity->id, x, y, 1, 0, hMatches);
        if(hCount >= 3){
          for(Entity* e : hMatches){
            if(!ContainsEntity(matchedEntities, e)){
              matchedEntities.push_back(e);
            }
          }
        }
        // vertical search
        vMatches.clear();
        int vCount = ComboCheck(lvl, entity->id, x, y, 0, 1, vMatches);
        if(vCount >= 3){
          for(Entity* e : vMatches){
            if(!ContainsEntity(matchedEntities, e)){
              matchedEntities.push_back(e);
            }
          }
        }
            
      }
    }
    if(matchedEntities.empty()){
      return false;
    }

    for(Entity* e : matchedEntities){
      e->id = ENTITY_ID::MATCHED;
    }

    int removed = RemoveMatches(lvl, matchedEntities);
    printf("Removed %d\n", removed);
    lvl->score += removed;
    printf("Score: %d\n", lvl->score);

    FallDown(lvl);

    SpawnFromTop(lvl);
    
    
    return true;
  }

  
  void UpdateGame(Gameplay* gameplay, Input* input, const float dt){
    Level* lvl = &gameplay->lvls[gameplay->currentLvlIndex];
    gameplay->animationInProgress = false;
    
    for(int i = 0; i < lvl->entityCount; i++){
      Entity& shape = lvl->shapeBuffer[i];
      if(shape.progress01 < 1.0f){
        shape.progress01 += dt / 1.5f;
        if(shape.progress01 > 1.0f){
          shape.progress01 = 1.0f;
        }
      }
      if(shape.id != ENTITY_ID::NONE && shape.progress01 < 1.0){
        gameplay->animationInProgress = true;
      }
    }
    if(gameplay->animationInProgress == false){
      if(ScanBoard(lvl)){
        gameplay->animationInProgress = true;
      }
    
    }

    // Mouse input
    if(MousePressed(input, MouseButtons::LEFT) && gameplay->animationInProgress == false){
      if(EntityClick(gameplay, lvl, input)){
        ScanBoard(lvl);  
      }
    }
    
  }

  /*
  void StartLevel(Gameplay* gameplay, Arena* arenaCommands, Arena* arenaEntities){
    Reset(arenaCommands);
    // CreateEntities(arenaEntities, &gameplay->lvls[gameplay->currentLvlIndex]);
  }
  */

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
      InitializeTitleScreen(data);
      data->transition.fadeTimeDuration = 4.0f;
      break;
    case SCENE_TYPES::MAINMENU:
      break;
    case SCENE_TYPES::GAME:{
      data->transition.fadeTimeDuration = 1.5f;
      Gameplay* gameplay = &data->scenes.gameplay;
      InitializeGame(&data->scenes.gameplay, data->arena_levels);
      assert(gameplay->initialized);
      InitializeLevel(data->arena_levels, &gameplay->lvls[gameplay->currentLvlIndex], 6, 9);    
      // StartLevel(gameplay, data->arena_commands, data->arena_entities);
      break;
    }
    case SCENE_TYPES::CREDITS:
      break;
    }
  }
  
  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    SetUpBoilerplate(data, window, renderer);

    
        
    ChangeScene(data, SCENE_TYPES::TITLESCREEN);

    PlaySong(SONG_ID::THEME);
    PlaySong(SONG_ID::WAVES);


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
            transition->fadeTimeElapsed = 0.0;
            break;
          case Transition::FadeFrom:
            transition->state = Transition::Inactive;
            break;
        }
      }
    }

    switch (data->sceneCurrent) {
      case SCENE_TYPES::TITLESCREEN:
        UpdateTitleScreen(titleScreen, &data->input, dt);
        if(KeyPressed(&data->input, SDL_SCANCODE_RETURN)){
          if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
            ChangeScene(data, SCENE_TYPES::GAME);
          }
        }
        break;

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:
        if(transition->state == Transition::Inactive){
          UpdateGame(gameplay, &data->input, dt);
        }
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
        SDL_RenderTexture(renderer, data->scenes.titleScreen.oceanTex, NULL, &data->scenes.titleScreen.oceanRect);
        SDL_RenderTexture(renderer, data->scenes.titleScreen.logoTex, NULL, &data->scenes.titleScreen.logoRect);
        break;
      }

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:{
        SDL_Texture* beachTex = GetSprite(SPRITE_ID::Beach, data->spriteBuffer)->texture;
        SDL_SetTextureScaleMode(beachTex, SDL_SCALEMODE_PIXELART);
        SDL_FRect beachRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderTexture(renderer, beachTex, NULL, &beachRect);
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        break;
      }
        
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

    // SDL_Texture* beachTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 240, 135);
    // SDL_Surface* beachSur = IMG_Load("assets/sprites/beach.png");
    
    RenderLevel(data, renderer);
    RenderEntities(data, renderer);
    
    // SDL_Texture* groundTex = GetSprite(SPRITE_ID::Background, data->spriteBuffer)->texture;
    // SDL_Texture* groundAltTex = GetSprite(SPRITE_ID::BackgroundAlt, data->spriteBuffer)->texture;
    // SDL_Texture* wallTex = GetSprite(SPRITE_ID::Wall, data->spriteBuffer)->texture;
    // SDL_Texture* squareTex = GetSprite(SPRITE_ID::Square, data->spriteBuffer)->texture;
    // SDL_Texture* sphereTex = GetSprite(SPRITE_ID::Sphere, data->spriteBuffer)->texture;
    // SDL_Texture* triangleTex = GetSprite(SPRITE_ID::Triangle, data->spriteBuffer)->texture;
    // SDL_Texture* plusTex = GetSprite(SPRITE_ID::Plus, data->spriteBuffer)->texture;
    // SDL_FRect squareRect = {(float)data->entity.xPos, (float)data->entity.yPos, 64, 64};

    
    // SDL_RenderTexture(renderer, squareTex, NULL, &squareRect);

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
