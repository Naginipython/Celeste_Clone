#include "main.h"
#include "window.h"
#include <GL/glew.h>
#include <SDL2/SDL_image.h>

static unsigned int create_shader(unsigned int type, const std::string& shaderPath) {
  unsigned int id = glCreateShader(type);
  std::string code = read_file(shaderPath);
  const char* src = code.c_str();
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  int success;
  char shaderLog[2048]{};
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(id, 2048, 0, shaderLog);
    glDeleteShader(id);
    ASSERT(false, "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << shaderLog);
  }
  return id;
}

void Window::load_shaders() {
  unsigned int vertShaderId = create_shader(GL_VERTEX_SHADER, "resources/shaders/quad.vert");
  unsigned int fragShaderId = create_shader(GL_FRAGMENT_SHADER, "resources/shaders/quad.frag");

  glAttachShader(glContext.programID, vertShaderId);
  glAttachShader(glContext.programID, fragShaderId);
  glLinkProgram(glContext.programID);

  glDetachShader(glContext.programID, vertShaderId);
  glDetachShader(glContext.programID, fragShaderId);
  glDeleteShader(vertShaderId);
  glDeleteShader(fragShaderId);

  time_t vertTime = get_timestamp("resources/shaders/quad.vert");
  time_t fragTime = get_timestamp("resources/shaders/quad.frag");
  glContext.shaderTimestamp = std::max(vertTime, fragTime);
}

void Window::init_gl() {
  // shaders
  glContext.programID = glCreateProgram();
  
  load_shaders();

  // Needed
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // image
  glGenTextures(1, &glContext.textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glContext.textureID);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
  glContext.textureTimestamp = get_timestamp(TEXTURE_PATH);

  SDL_FreeSurface(texture);
  texture = NULL;

  glEnable(GL_FRAMEBUFFER_SRGB);
  glDisable(0x809D);

  // Transform Storage Buffer
  glGenBuffers(1, &glContext.transformBufferId);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformBufferId); // For the input binding
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, renderData.transforms, GL_DYNAMIC_DRAW);

  // Uniforms
  glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
  glContext.projectionID = glGetUniformLocation(glContext.programID, "projection");

  // Depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  glUseProgram(glContext.programID);
}

void Window::gl_render() {
  // Texture Hot Reload
  time_t currTimestamp = get_timestamp(TEXTURE_PATH);
  if (currTimestamp > glContext.textureTimestamp) {
    glActiveTexture(GL_TEXTURE0);
    texture = IMG_Load(TEXTURE_PATH.c_str());
    if (texture) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
      SDL_FreeSurface(texture);
      texture = NULL;
      TRACE("Texture loaded");
      glContext.textureTimestamp = currTimestamp;
    } else {
      ERROR("Failed to load texture: " << IMG_GetError());
    }
  }

  // Shader Hot Reload
  time_t vertTime = get_timestamp("resources/shaders/quad.vert");
  time_t fragTime = get_timestamp("resources/shaders/quad.frag");
  if (std::max(vertTime, fragTime) > glContext.shaderTimestamp) {
    load_shaders();
    TRACE("Shader reloaded");
  }

  // OpenGL rendering
  SDL_GL_SwapWindow(window);
  glViewport(0, 0, get_width(), get_height());
  glClearColor(119.0/255.0, 33.0/255.0, 111.0/255.0, 1.0);
  glClearDepth(0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Copy uniform data
  Vec2 screenSize{(float)get_width(), (float)get_height()};
  glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

  Camera2D gameCamera = renderData.gameCamera;
  Matrix4 projection = camera_projection(
    gameCamera.position.x - gameCamera.dimentions.x / 2.0f, 
    gameCamera.position.x + gameCamera.dimentions.x / 2.0f, 
    gameCamera.position.y - gameCamera.dimentions.y / 2.0f, 
    gameCamera.position.y + gameCamera.dimentions.y / 2.0f
  );
  glUniformMatrix4fv(glContext.projectionID, 1, GL_TRUE, &projection.ax);

  // copies transform data to the buffer
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData.transformCount, renderData.transforms);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData.transformCount);

  renderData.transformCount = 0;
}