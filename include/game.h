#pragma once

#include "window.h"

class GameState {
 public:
  // todo
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