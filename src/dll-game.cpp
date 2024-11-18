#include "main.h"
#include "window.h"
#include "game.h"

EXPORT_FN void update_game(Window& window, GameState& gameState) {
  // for (int x = 0; x < 12; x++)
  //   for (int y = 0; y < 7; y++)
  //     window.draw_sprite(SPRITE_DICE, {100.0f * x, 100.0f * y}, {100.0f, 100.0f});
  window.renderData.gameCamera.position.x = 0;
  window.draw_sprite(SPRITE_DICE, {-160.0f, 0.0f});
}