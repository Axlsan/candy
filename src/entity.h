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
  PLUS = 7
};

struct Entity{
  ENTITY_ID id;
  int xPos;
  int yPos;
  int xPrev;
  int yPrev;
  float progress01;
  
};

