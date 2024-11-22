#include "game.h"

bool GameState::is_down(GameInputType input) {
  for (SDL_Scancode key : keyMapping[input].keys)
    if (keyStates[key]) return true;
  return false;
}

void GameState::handle_input(Window& window) {
  // Handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) window.isRunning = false;

    // Checks key up and down events
    if (event.type == SDL_KEYDOWN) {
      SDL_Keycode key = event.key.keysym.sym;
      keyStates[event.key.keysym.scancode] = true;
      if (key == SDLK_ESCAPE) window.isRunning = false;
    }
    if (event.type == SDL_KEYUP) {
      keyStates[event.key.keysym.scancode] = false;
    }

    // Checks mouse events
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      IVec2 mousePos = {event.button.x, event.button.y};
      if (event.button.button == SDL_BUTTON_LEFT)
        mouseStates[0] = window.screen_to_world(mousePos);
      if (event.button.button == SDL_BUTTON_RIGHT)
        mouseStates[1] = window.screen_to_world(mousePos);
    }
    if (event.type == SDL_MOUSEBUTTONUP) {
      if (event.button.button == SDL_BUTTON_LEFT)
        mouseStates[0] = {-1, -1};
      if (event.button.button == SDL_BUTTON_RIGHT)
        mouseStates[1] = {-1, -1};
    }
    if (event.type == SDL_MOUSEMOTION) {
      IVec2 mousePos = {event.motion.x, event.motion.y};
      if (mouseStates[0].x != -1 && mouseStates[0].y != -1)
        mouseStates[0] = window.screen_to_world(mousePos);
      if (mouseStates[1].x != -1 && mouseStates[1].y != -1)
        mouseStates[1] = window.screen_to_world(mousePos);
    }
  }

  // Handle input
  if (is_down(MOVE_UP)) playerPos.y -= 1;
  if (is_down(MOVE_DOWN)) playerPos.y += 1;
  if (is_down(MOVE_LEFT)) playerPos.x -= 1;
  if (is_down(MOVE_RIGHT)) playerPos.x += 1;
  if (mouseStates[0].x != -1 && mouseStates[0].y != -1) {
    Tile* tile = get_tile(mouseStates[0]);
    if (tile != nullptr) tile->isVisible = true;
    // TRACE("Clicked: " << mouseStates[0].x << ", " << mouseStates[0].y);
  }
  if (mouseStates[1].x != -1 && mouseStates[1].y != -1) {
    Tile* tile = get_tile(mouseStates[1]);
    if (tile != nullptr) tile->isVisible = false;
  }
}