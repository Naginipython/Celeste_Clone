#include "game.h"

double GameState::get_delta_time() {
  // Static makes it only execute once and remember the value
  static auto lastTime = std::chrono::high_resolution_clock::now();
  auto currTime = std::chrono::high_resolution_clock::now();

  double deltaTime = std::chrono::duration<double>(currTime - lastTime).count();
  lastTime = currTime;

  return deltaTime;
}

Tile* GameState::get_tile(int x, int y) {
  Tile* tile = nullptr;
  if(x >= 0  && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y) {
    tile = &worldGrid[x][y];
  }
  return tile;
}

Tile* GameState::get_tile(IVec2 pos) {
  int x = pos.x / TILESIZE;
  int y = pos.y / TILESIZE;
  return get_tile(x, y);
}