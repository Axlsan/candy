#pragma once

// #include "spriteLibrary.h"
#include <cstdint>

enum class ENTITY_ID : uint8_t{
  NONE = 0,
  BACKGROUND = 1,
  BACKGROUND_ALT = 2,
  WALL = 3,
  SQUARE = 4,
  SPHERE = 5,
  TRIANGLE = 6,
  PLUS = 7,
  MATCHED = 8
};

struct Entity{
  ENTITY_ID id;
  int xPos;
  int yPos;
  int xPrev;
  int yPrev;
  float progress01;
  
  bool scaleUp = false;
  float scale = 1;
  float scaleTimer = 0;

  Entity(){
    scale = 1.0;
    scaleUp = false;
    scaleTimer = 0.0;
  }
};

