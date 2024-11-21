#include "main.h"
#include "window.h"
#include "game.h"

Tile* get_tile(int x, int y, GameState& gameState) {
  Tile* tile = nullptr;
  if(x >= 0  && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y) {
    tile = &gameState.worldGrid[x][y];
  }
  return tile;
}

EXPORT_FN void update_game(Window& window, GameState& gameState) {
  window.draw_sprite(SPRITE_DICE, gameState.playerPos);

  handle_input(window, gameState);

  // Draw tilesets          
  // https://code.tutsplus.com/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673t
  //                        top    left   right  bot
  int neighborOffset[24] = {+0,-1, -1,+0, +1,+0, +0,+1,
  //                        topL   topR   botL   botR
                            -1,-1, +1,-1, -1,+1, +1,+1,
  //                        top2   left2  right2 bot2
                            +0,-2, -2,+0, +2,+0, +0,+2};
  // TopL = BIT(4) = 16
  // TopR = BIT(5) = 32
  // BotL = BIT(6) = 64
  // BotR = BIT(7) = 128
  for (int y = 0; y < WORLD_GRID.y; y++)
    for (int x = 0; x < WORLD_GRID.x; x++) {
      Tile* tile = get_tile(x, y, gameState);
      if (!tile->isVisible) continue;
      tile->neighborMask = 0;
      int neighborCount = 0;
      int extendedNeighborCount = 0;
      int emptyNeighborSlot = 0;
      
      // Check surrounding 12 neighbors
      for (int n = 0; n < 12; n++) {
        Tile* neighbor = get_tile(x + neighborOffset[n*2], y + neighborOffset[n*2 + 1], gameState);
        // No neighbor means the edge
        if (!neighbor || neighbor->isVisible) {
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
      // WARN("Should be 0 if alone: " << tile->neighborMask);
      transform.atlasOffset = gameState.atlasTileCoords[tile->neighborMask];
      window.draw_quad(transform);
      // Draw blank
      // Vec2 tilePos = {x*(float)TILESIZE + (float)TILESIZE /2.0f, y*(float)TILESIZE + (float)TILESIZE /2.0f};
      // window.draw_quad(tilePos, {8, 8});
    }
}