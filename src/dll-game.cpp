#include "main.h"
#include "window.h"
#include "game.h"

bool keyStates[SDL_NUM_SCANCODES] = {false};
IVec2 mouseStates[2] = {{-1, -1}, {-1, -1}};

bool is_down(GameInputType input, GameState& gameState) {
  for (SDL_Scancode key : gameState.keyMapping[input].keys)
    if (keyStates[key]) return true;
  return false;
}

Tile* get_tile(int x, int y, GameState& gameState) {
  Tile* tile = nullptr;
  if(x >= 0  && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y) {
    tile = &gameState.worldGrid[x][y];
  }
  return tile;
}
Tile* get_tile(IVec2 pos, GameState& gameState) {
  int x = pos.x / TILESIZE;
  int y = pos.y / TILESIZE;
  return get_tile(x, y, gameState);
}

EXPORT_FN void update_game(Window& window, GameState& gameState) {
  window.draw_sprite(SPRITE_DICE, gameState.playerPos);

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
  if (is_down(MOVE_UP, gameState)) gameState.playerPos.y -= 1;
  if (is_down(MOVE_DOWN, gameState)) gameState.playerPos.y += 1;
  if (is_down(MOVE_LEFT, gameState)) gameState.playerPos.x -= 1;
  if (is_down(MOVE_RIGHT, gameState)) gameState.playerPos.x += 1;
  if (mouseStates[0].x != -1 && mouseStates[0].y != -1) {
    Tile* tile = get_tile(mouseStates[0], gameState);
    if (tile != nullptr) tile->isVisible = true;
    // TRACE("Clicked: " << mouseStates[0].x << ", " << mouseStates[0].y);
  }
  if (mouseStates[1].x != -1 && mouseStates[1].y != -1) {
    Tile* tile = get_tile(mouseStates[1], gameState);
    if (tile != nullptr) tile->isVisible = false;
  }

  // Draw tilesets          
  // https://code.tutsplus.com/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673t
  //                        top    left   right  bot
  int neighborOffset[24] = {+0,-1, -1,+0, +1,+0, +0,+1
  //                        topL   topR   botL   botR
                            -1,-1, +1,-1, -1,+1, +1,+1
  //                        top2   left   right  bot2
                            +0,-2, -2,+0, +2,+0, +0,+2};
  // TopL = BIT(4) = 16
  // TopR = BIT(5) = 32
  // BotL = BIT(6) = 64
  // BotR = BIT(7) = 128
  for (int y = 0; y < WORLD_GRID.y; y++)
    for (int x = 0; x < WORLD_GRID.x; x++) {
      Tile* tile = get_tile(x, y, gameState);
      if (!tile->isVisible) continue;
      // Draw blank
      // Vec2 tilePos = {x*(float)TILESIZE + (float)TILESIZE /2.0f, y*(float)TILESIZE + (float)TILESIZE /2.0f};
      // window.draw_quad(tilePos, {8, 8});
      tile->neighborMask = 0;
      int neighborCount = 0;
      int extendedNeighborCount = 0;
      int emptyNeighborSlot = 0;
      
      // Check surrounding 12 neighbors
      for (int n = 0; n < 12; n++) {
        Tile* neighbor = get_tile(x + neighborOffset[n*2], y + neighborOffset[n*2 + 1], gameState);
        // No neighbor means the edge
        if (!neighbor || neighbor->isVisible) {
          TRACE("Neighbor: " << n);
          TRACE(!neighbor);
          TRACE(neighbor->isVisible);
          tile->neighborMask |= BIT(n);
          if (n < 8) neighborCount++;
          else extendedNeighborCount++;
        } else if (n < 8) emptyNeighborSlot = n;
      }

      if (neighborCount == 7 && emptyNeighborSlot >= 4) 
        tile->neighborMask = 16 + (emptyNeighborSlot - 4);
      else if (neighborCount == 8 && extendedNeighborCount == 4)
        tile->neighborMask = 20;
      else
        tile->neighborMask = tile->neighborMask & 0b1111;

      // Draw tile
      Transform transform{};
      transform.position = {x * (float)TILESIZE, y * (float)TILESIZE};
      transform.size = {8, 8};
      transform.spriteSize = {8, 8};
      WARN(tile->neighborMask);
      transform.atlasOffset = gameState.atlasTileCoords[tile->neighborMask];
      window.draw_quad(transform);
    }
}