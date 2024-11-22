#pragma once

#include <array>
#include <chrono>
#include <vector>

#include "window.h"

constexpr IVec2 WORLD_GRID = {WORLD_WIDTH / TILESIZE, WORLD_HEIGHT / TILESIZE};

enum GameInputType {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  JUMP,

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
 private:
  bool keyStates[SDL_NUM_SCANCODES] = {false};
  IVec2 mouseStates[2] = {{-1, -1}, {-1, -1}};
 public:
  // Fields
  IVec2 playerPos{0, 0};
  KeyMapping keyMapping[GAME_INPUT_COUNT];
  Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y]{};
  std::vector<IVec2> atlasTileCoords;

  // Methods
  GameState();
  double get_delta_time();
  Tile* get_tile(int x, int y);
  Tile* get_tile(IVec2 pos);
  bool is_down(GameInputType input);
  void handle_input(Window& window);
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
void handle_input(Window& window, GameState& gameState);