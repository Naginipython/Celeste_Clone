#pragma once

#include "window.h"
#include <array>
#include <vector>

constexpr IVec2 WORLD_GRID = {WORLD_WIDTH / TILESIZE, WORLD_HEIGHT / TILESIZE};

enum GameInputType {
  MOVE_LEFT, MOVE_RIGHT, 
  MOVE_UP, MOVE_DOWN,
  JUMP,

  MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE,
  GAME_INPUT_COUNT
};
struct KeyMapping {
  std::array<SDL_Scancode, 3> keys;
};

struct Tile {
  int neighborMask{};
  bool isVisible{false};
};

class GameState {
 public:
  GameState() {
    // Set keymap
    keyMapping[MOVE_LEFT].keys = {SDL_SCANCODE_A, SDL_SCANCODE_LEFT};
    keyMapping[MOVE_RIGHT].keys = {SDL_SCANCODE_D, SDL_SCANCODE_RIGHT};
    keyMapping[MOVE_UP].keys = {SDL_SCANCODE_W, SDL_SCANCODE_UP};
    keyMapping[MOVE_DOWN].keys = {SDL_SCANCODE_S, SDL_SCANCODE_DOWN};
    // keyMapping[JUMP] = {{SDL_SCANCODE_SPACE}};

    // Set atlas tile coords
    IVec2 tileStartPos = {48, 0};
    for (int y = 0; y < 5; y++)
      for (int x = 0; x < 4; x++) 
        atlasTileCoords.push_back({tileStartPos.x + x * 8, tileStartPos.y + y * 8});
    atlasTileCoords.push_back({tileStartPos.x, tileStartPos.y + 5 * 8});
  }
  IVec2 playerPos{0, 0};
  KeyMapping keyMapping[GAME_INPUT_COUNT];
  Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
  std::vector<IVec2> atlasTileCoords;
};

#ifdef _WIN32
  #define EXPORT_FN __declspec(dllexport)
#elif __linux__
  #define EXPORT_FN __attribute__((visibility("default")))
#elif __APPLE__
  #define EXPORT_FN __attribute__((visibility("default")))
#endif

extern "C" {
  EXPORT_FN void update_game(Window& window, GameState& gameState);
}