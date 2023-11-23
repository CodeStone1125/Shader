#include "gl_helper.h"
#include <fstream>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint quickCreateProgram(const char* vert_shader_filename, const char* frag_shader_filename) {
  GLuint vert = createShader(vert_shader_filename, GL_VERTEX_SHADER);
  if (vert == 0) return 0;

  GLuint frag = createShader(frag_shader_filename, GL_FRAGMENT_SHADER);
  if (frag == 0) {
    glDeleteShader(vert);
    return 0;
  }

  GLuint prog = createProgram(vert, frag);
  if (prog == 0) {
    glDeleteShader(vert);
    glDeleteShader(frag);
    return 0;
  }
  return prog;
}

GLuint createShader(const char* filename, GLenum type) {
  // Read shader code
  char* buffer = 0;
  long length;
  std::ifstream infile(filename, std::ios::binary);
  if (!infile.is_open()) {
    std::cout << "Open file fail: " << filename << std::endl;
    return 0;
  }
  infile.seekg(0, std::ios::end);
  length = (long)infile.tellg();
  infile.seekg(0, std::ios::beg);
  buffer = (char*)malloc(length + 1);
  buffer[length] = 0;
  if (buffer == NULL) {
    std::cout << "Allocate memory fail" << std::endl;
    infile.close();
    return 0;
  }
  infile.read(buffer, length);
  infile.close();

  // Compile shader
  int success;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar**)&buffer, 0);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    glDeleteShader(shader);
    shader = 0;
  };

  free(buffer);
  return shader;
}

GLuint createProgram(GLuint vert, GLuint frag) {
  // shader Program
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  // print linking errors if any
  int success;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(prog, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    glDeleteProgram(prog);
    return 0;
  }

  // Always detach shaders after a successful link.
  glDetachShader(prog, vert);
  glDetachShader(prog, frag);
  return prog;
}

GLuint createTexture(const char* filename) {
  GLuint texture;
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data == NULL) {
    std::cout << "Failed to load texture " << filename << std::endl;
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return texture;
}
