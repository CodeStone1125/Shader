#pragma once

#include <glad/gl.h>
#include "gl_helper.h"

class Context;

class Program {
 public:
  const char *vertProgramFile;
  const char *fragProgramFIle;

 public:
  Program(Context *ctx) : ctx(ctx) {
    vertProgramFile = "../assets/shaders/example.vert";
    fragProgramFIle = "../assets/shaders/example.frag";
  }

  virtual bool load() = 0;
  virtual void doMainLoop() = 0;

 protected:
  GLuint programId = -1;
  const Context *ctx;
  GLuint *VAO = 0;
};

class ExampleProgram : public Program {
 public:
  ExampleProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/example.vert";
    fragProgramFIle = "../assets/shaders/example.frag";
  }
  bool load() override;
  void doMainLoop() override;
};

class BasicProgram : public Program {
 public:
  BasicProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/basic.vert";
    fragProgramFIle = "../assets/shaders/basic.frag";
  }
  bool load() override;
  void doMainLoop() override;
};

class LightProgram : public BasicProgram {
 public:
  LightProgram(Context *ctx) : BasicProgram(ctx) {
    vertProgramFile = "../assets/shaders/light.vert";
    fragProgramFIle = "../assets/shaders/light.frag";
  }
  bool load() override;
  void doMainLoop() override;
};