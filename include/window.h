#pragma once
#include "main.h"
#include "sprites.h"
#include <SDL2/SDL.h>
#include <string>

struct GLContext {
  unsigned int programID{};
  unsigned int textureID{};
  unsigned int transformBufferId{};
  unsigned int screenSizeID{};
  unsigned int projectionID{};
};

struct Camera2D {
  float zoom{1.0f};
  Vec2 dimentions;
  Vec2 position;
};

struct RenderData {
  Camera2D gameCamera;
  Camera2D uiCamera;

  int transformCount;
  Transform transforms[MAX_TRANSFORMS];

  inline RenderData() {
    gameCamera.dimentions = {WORLD_WIDTH, WORLD_HEIGHT};
  }
};

class Window {
 private:
  std::string TEXTURE_PATH{"resources/textures/TEXTURE_ATLAS.png"};
  SDL_Window* window;
  SDL_GLContext sdlGlContext;
  GLContext glContext{};
  SDL_Surface* texture;

 public:
  RenderData renderData;
  bool isRunning{true};
  Window(int x, int y, const std::string& title);
  ~Window();
  const inline int get_width() const { int w, h; SDL_GetWindowSize(window, &w, &h); return w; }
  const inline int get_height() const { int w, h; SDL_GetWindowSize(window, &w, &h); return h; }
  void init_gl();
  void update();
  void gl_render();
  void draw_sprite(SpriteID id, Vec2 pos);
  void draw_sprite(SpriteID id, IVec2 pos);
  Vec2 screen_to_world(Vec2 screenPos);
};