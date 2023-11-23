#include <iostream>

#include "context.h"
#include "program.h"

bool BasicProgram::load() {
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);

  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];

  /* TODO#2-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to VBOs
   * Note:
   *           1. You may refer to Programs/example.cpp and example.vert
   *           2. You may checkout basic.vert to get location index first
   *           3. You can get model data from ctx->models[i]->{posisions,normals,texcoords}
   * Keywords:
   *           - glGenVertexArrays
   *           - glBindVertexArray
   *           - glGenBuffers
   *           - glBufferData
   *           - glEnableVertexAttribArray
   *           - glVertexAttribPointer
   */

  return programId != 0;
}

void BasicProgram::doMainLoop() {
  /* TODO#2-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass model texture to shaders
   *           6. Draw with glDrawArrays
   * Note:
   *           1. You may refer to Programs/example.cpp and example.vert
   *           2. You may checkout definition of Context class, Model class and Object class first
   *           3. You can get object class from ctx->objects[i]
   *           4. You can get texture index of Model::textures for current object by Object::textureIndex
   *           5. you may also need model->drawMode, model->numVertex
   * Keywords:
   *           - glUseProgram
   *           - glBindVertexArray
   *           - glGetUniformLocation
   *           - glUniformMatrix4fv
   *           - glActiveTexture
   *           - glBindTexture
   *           - glUniform1i
   *           - glDrawArrays
   */

  
  glUseProgram(0);
}