#include <iostream>
#include "context.h"
#include "program.h"

bool LightProgram::load() {
  /* TODO#4-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to to VBOs
   * Note:
   *           If you implament BasicProgram properly, You might inherent BasicProgram's load function
   */
  return programId != 0;
}

void LightProgram::doMainLoop() {
  /* TODO#4-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass light and material parameters to shader
   *           6. Pass model texture to shaders
   *           7. Draw with glDrawArrays
   * Note:
   *           1. light paramters are providered in context.h
   *           2. material parameter for each object get be found in ctx->objects[i]->material
   */

  glUseProgram(0);
}
