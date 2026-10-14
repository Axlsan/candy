#pragma once

#include <cmath>
#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define loop(i, e) for(int i = 0; i < e; i++)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(14);
constexpr size_t AUDIO_MEMORY_ALLOWANCE = MEGABYTES(5);
constexpr int FPS = 240;
constexpr double FRAME_TIME_MS = 1000.0 / FPS;

const float UNDO_REPEAT_TIME = 0.15;
const int SCREEN_WIDTH = 1920 / 3 * 2;
const int SCREEN_HEIGHT = 1080 / 3 * 2;
const int UPSCALE_FACTOR = 4;
const int SPRITE_SIZE = 8;

const int TILE_SIZE_PX_RAW = 16;
const int TILE_SIZE_PX_SCALED = TILE_SIZE_PX_RAW * UPSCALE_FACTOR;


inline void Convert1DTo2D(int flatIndex, int width, int* x, int* y){
    *x = flatIndex % width;
    *y = flatIndex / width;
}
inline void Convert1DTo2D(int flatIndex, int width, float* x, float* y){
    *x = (float)(flatIndex % width);
    *y = (float)(flatIndex / width);
}

static const char STOP_CHAR = '\0';
inline bool IsStringEmpty(const char* str){
    return str == nullptr || str[0] == STOP_CHAR;
}

inline float SnapToGrid(float value){
  return ((int)(value / TILE_SIZE_PX_SCALED) * TILE_SIZE_PX_SCALED) + (TILE_SIZE_PX_SCALED / UPSCALE_FACTOR);
}

// Curves
inline float Lerp(float current, float target, float dt){
    return current + (target - current) * dt;
}
inline float EaseIn(float t){
        return t * t;
}
inline float EaseInOut(float t){
        return t * t;
}
inline float EaseInOutBack(float t){
        constexpr float c1 = 1.70158f;
        constexpr float c2 = c1 * 1.525f;
    
        return t < 0.5f
            ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
            : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (2.0f * t - 2.0f) + c2) + 2.0f) / 2.0f;
}
