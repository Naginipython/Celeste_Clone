#include "main.h"
#include "window.h"
#include "game.h"

EXPORT_FN void update_game(Window& window) {
  for (int x = 0; x < 12; x++)
    for (int y = 0; y < 7; y++)
      window.draw_sprite(SPRITE_DICE, {100.0f * x, 100.0f * y}, {100.0f, 100.0f});
}