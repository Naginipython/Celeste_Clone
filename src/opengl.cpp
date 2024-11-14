#include "main.h"
#include "window.h"
#include <GL/glew.h>

void Window::init_gl() {
  // shaders
  unsigned int vertShaderId = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

  std::string vertShaderCode = read_file("resources/shaders/quad.vert");
  std::string fragShaderCode = read_file("resources/shaders/quad.frag");
  const char* vertShaderCodeC = vertShaderCode.c_str();
  const char* fragShaderCodeC = fragShaderCode.c_str();

  glShaderSource(vertShaderId, 1, &vertShaderCodeC, NULL);
  glShaderSource(fragShaderId, 1, &fragShaderCodeC, NULL);

  glCompileShader(vertShaderId);
  glCompileShader(fragShaderId);

  // Check for errors
  int success;
  char infoLog[2048]{};
  glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertShaderId, 2048, NULL, infoLog);
    ASSERT(false, "Failed to compile vertex shader: " << infoLog);
  }
  glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragShaderId, 2048, NULL, infoLog);
    ASSERT(false, "Failed to compile fragment shader: " << infoLog);
  }

  glContext.programID = glCreateProgram();
  glAttachShader(glContext.programID, vertShaderId);
  glAttachShader(glContext.programID, fragShaderId);
  glLinkProgram(glContext.programID);

  glDetachShader(glContext.programID, vertShaderId);
  glDetachShader(glContext.programID, fragShaderId);
  glDeleteShader(vertShaderId);
  glDeleteShader(fragShaderId);

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

  SDL_FreeSurface(texture);

  glEnable(GL_FRAMEBUFFER_SRGB);
  glDisable(0x809D);

  // Transform Storage Buffer
  glGenBuffers(1, &glContext.transformBufferId);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformBufferId); // For the input binding
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, renderData.transforms, GL_DYNAMIC_DRAW);

  // Uniforms
  glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");

  // Depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  glUseProgram(glContext.programID);
}

void Window::gl_render() {
  SDL_GL_SwapWindow(window);
  glViewport(0, 0, get_width(), get_height());
  glClearColor(119.0/255.0, 33.0/255.0, 111.0/255.0, 1.0);
  glClearDepth(0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Copy uniform data
  Vec2 screenSize{(float)get_width(), (float)get_height()};
  glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

  // copies transform data to the buffer
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData.transformCount, renderData.transforms);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData.transformCount);

  renderData.transformCount = 0;
}