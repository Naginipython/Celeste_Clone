#pragma once
#include "main.h"
#include "sprites.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

struct GLContext {
  unsigned int programID{};
  unsigned int textureID{};
  unsigned int transformBufferId{};
  unsigned int screenSizeID{};
  unsigned int projectionID{};

  time_t textureTimestamp{};
  time_t shaderTimestamp{};
};

struct Camera2D {
  float zoom{1.0f};
  Vec2 dimentions;
  Vec2 position;
};

struct RenderData {
  Camera2D gameCamera;
  Camera2D uiCamera;

  std::vector<Transform> transforms;

  inline RenderData() {
    gameCamera.dimentions = {WORLD_WIDTH, WORLD_HEIGHT};
    // Sets camera back to 0,0 as top left
    gameCamera.position = {WORLD_WIDTH/2, -WORLD_HEIGHT/2};
  }
};

class Window {
 private:
  std::string TEXTURE_PATH{"resources/textures/TEXTURE_ATLAS.png"};
  SDL_Window* window;
  SDL_GLContext sdlGlContext;
  GLContext glContext{};
  SDL_Surface* texture;

  void load_shaders();
 public:
  RenderData renderData;
  bool isRunning{true};
  Window(int x, int y, const std::string& title);
  ~Window();
  void init_gl();

  const inline int get_width() const { int w, h; SDL_GetWindowSize(window, &w, &h); return w; }
  const inline int get_height() const { int w, h; SDL_GetWindowSize(window, &w, &h); return h; }
  
  void gl_render();
  void draw_sprite(SpriteID id, Vec2 pos);
  void draw_sprite(SpriteID id, IVec2 pos);
  void draw_quad(Vec2 pos, Vec2 size);
  void draw_quad(Transform transform);
  Vec2 screen_to_world(Vec2 screenPos);
  IVec2 screen_to_world(IVec2 screenPos);
};