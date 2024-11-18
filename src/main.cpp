#include "main.h"
#include "window.h"
#include "game.h"
#include <dlfcn.h>
#include <thread>
#include <chrono>

// DLL stuff
typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;
void reload_dll();

int main(int argc, char* argv[]) {
  Window window{1200, 720, "Celeste Clone"};
  
  while (window.isRunning) {
    reload_dll();
    update_game(window);
    window.update();
  }
  
  return 0;
}

void update_game(Window& window) {
  update_game_ptr(window);
}

void reload_dll() {
  static void* gameDLL;
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
    TRACE("Reloaded libgame.so");
    lastWrite = timestamp;
  }
}