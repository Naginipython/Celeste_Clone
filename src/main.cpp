#include "main.h"

#include <dlfcn.h>

#include "game.h"
#include "window.h"

// DLL stuff
typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr = nullptr;
void reload_dll();

int main(int argc, char* argv[]) {
  Window window{WORLD_WIDTH * 4, WORLD_HEIGHT * 4, "Celeste Clone"};
  GameState gameState{};

  while (window.isRunning) {
    reload_dll();
    update_game(window, gameState);
  }

  return 0;
}

// Init GameState
GameState::GameState() {
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
      atlasTileCoords.push_back(
          {tileStartPos.x + x * 8, tileStartPos.y + y * 8});
  atlasTileCoords.push_back({tileStartPos.x, tileStartPos.y + 5 * 8});
  // checking world grid
  for (int y = 0; y < WORLD_GRID.y; y++)
    for (int x = 0; x < WORLD_GRID.x; x++) {
      worldGrid[x][y].isVisible = false;
      worldGrid[x][y].neighborMask = 0;
    }
}

void update_game(Window& window, GameState& gameState) {
  if (update_game_ptr)
    update_game_ptr(window, gameState);
  else {
    ERROR("update_game_ptr is null");
    exit(1);
  }
}

// TODO: System dependance
void reload_dll() {
  static void* gameDLL = nullptr;
  static time_t lastWrite;

  long timestamp = get_timestamp("./zig-out/lib/libgame.so");
  if (timestamp > lastWrite) {
    // Delete prev lib
    if (gameDLL) {
      int did_close = dlclose(gameDLL);
      ASSERT(did_close == 0, "Failed to close libgame.so");
      gameDLL = nullptr;
      TRACE("Freed libgame.so");
    }

    // Load lib
    gameDLL = dlopen("./zig-out/lib/libgame.so", RTLD_LAZY);
    if (!gameDLL) {
      println("Failed to load libgame.so: " << dlerror());
    }

    // Load function
    update_game_ptr = (update_game_type*)dlsym(gameDLL, "update_game");
    if (!update_game_ptr) {
      println("Failed to load symbol update_game: " << dlerror());
      dlclose(gameDLL);
    }
    TRACE("Loaded libgame.so");
    lastWrite = timestamp;
  }
}