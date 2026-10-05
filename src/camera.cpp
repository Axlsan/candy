

#include "camera.h"
#include "common.h"
bool camera::IsPointInGrid(float x, float y, const Level* lvl){
  int gridX;
  int gridY;
  WorldToGrid(x, y, &gridX, &gridY, lvl);
  return gridX >= 0 && gridY >= 0 && gridX < lvl->w && gridY < lvl->h;
}

void camera::GridToWorld(float* x, float* y, const Level* lvl){
  *x *= TILE_SIZE_PX_SCALED;
  *x += SCREEN_WIDTH / 2;
  *x -= lvl->w * TILE_SIZE_PX_SCALED / 2;
  *y *= TILE_SIZE_PX_SCALED;
  *y += SCREEN_HEIGHT / 2;
  *y -= lvl->h * TILE_SIZE_PX_SCALED / 2;
}

void camera::WorldToGrid(float worldX, float worldY, int* x, int* y, const Level* lvl){
  *x = worldX;
  *y = worldY;
  
  *x += lvl->w * TILE_SIZE_PX_SCALED / 2;
  *x -= SCREEN_WIDTH / 2;
  *x /= TILE_SIZE_PX_SCALED;
  *y += lvl->h * TILE_SIZE_PX_SCALED / 2;
  *y -= SCREEN_HEIGHT / 2;
  *y /= TILE_SIZE_PX_SCALED;
}
