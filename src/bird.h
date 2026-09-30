#pragma once

struct Bird{
  float x;
  float y;
  float velocity;
  static constexpr float MAX_VELOCITY = 5;
  static constexpr float FLAP_STR = -3.3;
};
