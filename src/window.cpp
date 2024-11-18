#include "window.h"

#include <GL/glew.h>
#include <SDL2/SDL_image.h>

#include "main.h"

Window::Window(int width, int height, const std::string& title) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    ERROR("Failed to initialize SDL: " << SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  ASSERT(window != NULL, "Failed to create window" << SDL_GetError());

  sdlGlContext = SDL_GL_CreateContext(window);
  ASSERT(sdlGlContext != NULL,
         "Failed to create OpenGL context: " << SDL_GetError());

  ASSERT(glewInit() == GLEW_OK,
         "Failed to initialize GLEW: " << glewGetErrorString(glGetError()));

  ASSERT(IMG_Init(IMG_INIT_PNG) >= 0,
         "Failed to initialize SDL_image: " << IMG_GetError());

  texture = IMG_Load(TEXTURE_PATH.c_str());
  ASSERT(texture != NULL, "Failed to load texture: " << IMG_GetError());

  init_gl();

  TRACE("OpenGL Version: " << glGetString(GL_VERSION));
}

Window::~Window() {
  glDeleteProgram(glContext.programID);
  if (texture != NULL) SDL_FreeSurface(texture);
  SDL_GL_DeleteContext(sdlGlContext);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}

void Window::update() {

  // Render
  gl_render();

  // Handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) isRunning = false;

    if (event.type == SDL_KEYDOWN) switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          isRunning = false;
          break;
        default:
          println(event.key.keysym.sym);
          break;
      }
  }
}

void Window::draw_sprite(SpriteID id, Vec2 pos, Vec2 size) {
  Sprite sprite = get_Sprite(id);

  Transform transform{};
  transform.atlasOffset = sprite.atlasOffset;
  transform.spriteSize = sprite.spriteSize;
  transform.position = pos;
  transform.size = size;

  renderData.transforms[renderData.transformCount++] = transform;
}
void Window::test() {
  TRACE("test");
}