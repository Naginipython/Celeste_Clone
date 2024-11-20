#include "main.h"
#include "window.h"
#include <GL/glew.h>

unsigned int create_shader(unsigned int type, const std::string& code) {
  unsigned int id = glCreateShader(type);
  const char* src = code.c_str();
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    ERROR("Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader");
    glDeleteShader(id);
    ASSERT(false, "Failed to compile vertex shader: " << message);
    return 0;
  }
  return id;
}

void Window::init_gl() {
  // shaders
  std::string vertShaderCode = read_file("resources/shaders/quad.vert");
  std::string fragShaderCode = read_file("resources/shaders/quad.frag");
  unsigned int vertShaderId = create_shader(GL_VERTEX_SHADER, vertShaderCode);
  unsigned int fragShaderId = create_shader(GL_FRAGMENT_SHADER, fragShaderCode);

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