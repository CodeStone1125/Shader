#include <iostream>
#include "context.h"
#include "program.h"

bool LightProgram::load() {
  /* TODO#4-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals, and texcoords to VBOs
   * Note:
   *           If you implemented BasicProgram properly, You might inherit BasicProgram's load function
   */

  // Assuming you have a function like createVAOAndBindAttributes in your BasicProgram
  // that sets up the VAO and VBOs
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);

  int num_models = (int)ctx->models.size();
  VAO = new GLuint[num_models];
  glGenVertexArrays(num_models, VAO);

  for (int i = 0; i < num_models; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[3];
    glGenBuffers(3, VBO);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);

    // Texcoords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(2);
  }

  return programId != 0;
}

void LightProgram::doMainLoop() {
  /* TODO#4-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load current model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass light and material parameters to shader
   *           6. Pass model texture to shaders
   *           7. Draw with glDrawArrays
   * Note:
   *           1. light parameters are provided in context.h
   *           2. material parameter for each object can be found in ctx->objects[i]->material
   */

   // 1. use and bind program (BasicProgram::programId)
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  // 2. Iterate all objects (ctx->objects)
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;

    // 3. Load current model VAO for object
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];
    // 4. Pass projection, view, model matrix to shaders
    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    // 5. Pass light and material parameters to shader
    // unifrom Material material
    glUniform3fv(glGetUniformLocation(programId, "material.ambient"), 1,
                 glm::value_ptr(ctx->objects[i]->material.ambient));
    glUniform3fv(glGetUniformLocation(programId, "material.diffuse"), 1,
                 glm::value_ptr(ctx->objects[i]->material.diffuse));
    glUniform3fv(glGetUniformLocation(programId, "material.specular"), 1,
                 glm::value_ptr(ctx->objects[i]->material.specular));
    glUniform1f(glGetUniformLocation(programId, "material.shininess"), ctx->objects[i]->material.shininess);

    // uniform PointLight pl
    glUniform1i(glGetUniformLocation(programId, "pl.enable"), ctx->pointLightEnable);
    glUniform3fv(glGetUniformLocation(programId, "pl.position"), 1, glm::value_ptr(ctx->pointLightPosition));
    glUniform3fv(glGetUniformLocation(programId, "pl.lightColor"), 1, glm::value_ptr(ctx->pointLightColor));
    glUniform1f(glGetUniformLocation(programId, "pl.constant"), ctx->pointLightConstant);
    glUniform1f(glGetUniformLocation(programId, "pl.linear"), ctx->pointLightLinear);
    glUniform1f(glGetUniformLocation(programId, "pl.quadratic"), ctx->pointLightQuardratic);


// uniform Spotlight sl
    glUniform1i(glGetUniformLocation(programId, "sl.enable"), ctx->spotLightEnable);
    glUniform3fv(glGetUniformLocation(programId, "sl.position"), 1, glm::value_ptr(ctx->spotLightPosition));
    glUniform3fv(glGetUniformLocation(programId, "sl.direction"), 1, glm::value_ptr(ctx->spotLightDirection));
    glUniform3fv(glGetUniformLocation(programId, "sl.lightColor"), 1, glm::value_ptr(ctx->spotLightColor));
    glUniform1f(glGetUniformLocation(programId, "sl.cutOff"), ctx->spotLightCutOff);
    glUniform1f(glGetUniformLocation(programId, "sl.constant"), ctx->spotLightConstant);
    glUniform1f(glGetUniformLocation(programId, "sl.linear"), ctx->spotLightLinear);
    glUniform1f(glGetUniformLocation(programId, "sl.quadratic"), ctx->spotLightQuardratic);


    // uniform DirectionLight dl
    glUniform1i(glGetUniformLocation(programId, "dl.enable"), ctx->directionLightEnable);
    glUniform3fv(glGetUniformLocation(programId, "dl.direction"), 1, glm::value_ptr(ctx->directionLightDirection));
    glUniform3fv(glGetUniformLocation(programId, "dl.lightColor"), 1, glm::value_ptr(ctx->directionLightColor));


    // 6. Pass model texture to shaders
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);

    // Pass texture unit to shader
    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);

    // 7. Draw with glDrawArrays
    glDrawArrays(model->drawMode, 0, model->numVertex);

    // Unbind the vertex array after drawing
    glBindVertexArray(0);
  }

  glUseProgram(0);
}
