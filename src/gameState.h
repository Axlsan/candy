#pragma once
#include "arena.h"
#include "audioSystem.h"
#include "bird.h"
#include "camera.h"
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

enum class GAME_STATE{
  GAME,
  PAUSED,
  GAME_OVER
};

struct GameData {
  Entity* shape;
  Level level;
  int score;
  GAME_STATE gameState;
  float game_over_timer;
  float grace_timer;

  EditorData editor_data;
  Input input;
  Sprite* spriteBuffer;
  AudioSystem audio;
  FontAtlas font; // not relevant
  bool running;
  Memory::Arena* arena_main;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_input;
  Memory::Arena* arena_scratch;
  Camera camera;
  ImGuiContext* imGui_context;
  const float* dt;  
  float* dt_scaler;
 };
