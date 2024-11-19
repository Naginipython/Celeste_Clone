#include "main.h"
#include "window.h"
#include "game.h"

bool keyStates[SDL_NUM_SCANCODES] = {false};

bool is_down(GameInputType input, GameState& gameState) {
  for (SDL_Scancode key : gameState.keyMapping[input].keys)
    if (keyStates[key]) return true;
  return false;
}

EXPORT_FN void update_game(Window& window, GameState& gameState) {
  window.renderData.gameCamera.position.x = 0;
  window.draw_sprite(SPRITE_DICE, gameState.playerPos);

  // Handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) window.isRunning = false;

    if (event.type == SDL_KEYDOWN) {
      SDL_Keycode key = event.key.keysym.sym;
      keyStates[event.key.keysym.scancode] = true;
      if (key == SDLK_ESCAPE) window.isRunning = false;
    }

    if (event.type == SDL_KEYUP) {
      keyStates[event.key.keysym.scancode] = false;
    }
  }

    // Handle input
  if (is_down(MOVE_UP, gameState)) gameState.playerPos.y -= 1;
  if (is_down(MOVE_DOWN, gameState)) gameState.playerPos.y += 1;
  if (is_down(MOVE_LEFT, gameState)) gameState.playerPos.x -= 1;
  if (is_down(MOVE_RIGHT, gameState)) gameState.playerPos.x += 1;
}