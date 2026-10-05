#pragma once
#include "arena.h"
#include "audioSystem.h"
#include "camera.h"
#include "command.h"
#include "fontLibrary.h"
#include "imgui/imgui_internal.h"
#include "input.h"
#include "level.h"
#include "spriteLibrary.h"

struct EditorData{
  float* fps_buffer;
  int fps_buffer_count;
  int fps_buffer_index;
};

struct Gameplay{
  CommandBuffer* commandBuffer;
  Level* lvls;
  int lvlCount;
  int currentLvlIndex;
  // Position* inputBuffer;
  // int inputBufferCapacity;
  // int inputBufferWriteCount;
  // int inputBufferReadCount;
  bool initialized;
};

struct MainMenu{
  
};

struct TitleScreen{
  
};

struct Credits{
  
};

enum class SCENE_TYPES{
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS
};

struct Scenes{
  Gameplay gameplay;
  MainMenu mainMenu;
  TitleScreen titleScreen;
  Credits credits;
};

struct Transition{
  enum States{
    Inactive,
    FadeTo,
    FadeFrom
  };
  States state;
  float fadeTimeElapsed;
  float fadeTimeDuration = 1;
};

// struct EditorData{
//   bool editorLevel;
//   Editor editor;
// };

struct GameData {

  SCENE_TYPES sceneCurrent;
  SCENE_TYPES scenePrevious;
  Scenes scenes;
  Transition transition;

  
  EditorData editor_data;
  Input input;
  Sprite* spriteBuffer;
  AudioSystem audio;
  FontAtlas font; // not relevant
  bool running;
  Memory::Arena* arena_main;
  Memory::Arena* arena_images;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_input;
  Memory::Arena* arena_scratch;
  Camera camera;
  ImGuiContext* imGui_context;
  const float* dt;  
  float* dt_scaler;

  // Level* levels;
  // int currentLevelIndex;
  Entity entity;
  int levelCount;
 };

/*
inline Level* GetCurentLevel(gameplay* game){
  return &game->levels[currentLevelIndex];
}
*/
