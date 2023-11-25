# NYCU CG2023 Homework 2

* Note: You can press `ctrl + shift + f` and search `TODO` to know where to implement!!!

## Dependencies

- [glfw](https://github.com/glfw/glfw)
- [glad](https://github.com/Dav1dde/glad)
- [glm](https://github.com/g-truc/glm)

### Dependencies for Windows

Visual Studio

### Dependencies for macOS

Xcode

### Dependencies for Unix-like systems other than macOS with X11

On *Debian* and derivatives like *Ubuntu* and *Linux Mint*

`sudo apt install xorg-dev`

On *Fedora* and derivatives like *Red Hat*

`sudo dnf install libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel`

On *FreeBSD*

`pkg install xorgproto`

## Build instruction

### CMake

Build in release mode
```bash=
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 8
cd bin
./HW2
```

Build in debug mode
```bash=
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel 8
cd bin
./HW2
```

### Visual Studio 2019

- Open `vs2019/HW2.sln`
- Select config then build (CTRL+SHIFT+B)
- Use F5 to debug or CTRL+F5 to run.


### Assets soruce
+ https://www.cgtrader.com/free-3d-models/textures/miscellaneous/wood-texture-01
+ https://www.cgtrader.com/free-3d-models/household/kitchenware/mug-set-f1930d59-e3a7-42fa-8634-ac8a44449dc6
+ https://www.cgtrader.com/free-3d-models/various/various-models/default-cube-fd46b883-71f4-443f-877d-3669dc0ad9ce

# Report
## Implementation(HOW & WHY)
### Task#1 OBJ file parser (10%)
Thanks to TA for writing the code to push necessary files like the obj file and texture. Therefore, defining 
how to handle the obj file is the only thing I need to do in Task#1. I use four if-else conditions to deal with v, vn,
vt, and f respectively. Here is my code in `model.cpp`:

```cpp
  /* TODO#1: Load model data from OBJ file */
  // Arrays to store vertex data
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;

  // Temporary variables to store face indices
  std::vector<int> positionIndices;
  std::vector<int> normalIndices;
  std::vector<int> texcoordIndices;

  std::string line;
  while (std::getline(ObjFile, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "v") {
      glm::vec3 vertex;
      iss >> vertex.x >> vertex.y >> vertex.z;
      positions.push_back(vertex);
    } else if (type == "vn") {
      glm::vec3 normal;
      iss >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
    } else if (type == "vt") {
      glm::vec2 texcoord;
      iss >> texcoord.x >> texcoord.y;
      texcoords.push_back(texcoord);
    } else if (type == "f") {
      int vertexIndex, texcoordIndex, normalIndex;
      char slash;  // to handle slashes in face data

      // 用於存儲面的頂點數量
      int vertexCount = 0;

      // 使用 do-while 循環讀取所有的頂點
      do {
        iss >> vertexIndex;

        // 將每個頂點的索引添加到相應的 vector 中
        positionIndices.push_back(vertexIndex - 1);

        // 檢查下一個字符是否是 '/'
        if (iss.peek() == '/') {
          iss.ignore();  // 忽略 '/'
          // 檢查是否有法線坐標
          if (iss.peek() != '/') {
            iss >> texcoordIndex;
            texcoordIndices.push_back(texcoordIndex - 1);
          }
          // 檢查是否有法線坐標
          if (iss.peek() == '/') {
            iss.ignore();  // 忽略 '/'
            iss >> normalIndex;
            normalIndices.push_back(normalIndex - 1);
          }
        }

        // 增加頂點數量
        vertexCount++;

        // 跳過空白字符
        iss >> std::ws;

      } while (iss.good());
    }
```

### Task#2 Basic shader program (20%)
I will divide this task in to three parts:
1. basic.vert
2. basic.frag
3. basic.cpp

#### basic.vert
I passed `gl_Position` and performed computations using `Projection`, `ViewMatrix`, and `ModelMatrix` to
obtain the correct position in the scene. Additionally, I passed the `out vec2 TexCoord` to the `layout
(location = 2) in vec2 texCoord` in the vertex shader, allowing the transfer of texture coordinates to the
fragment shader.

```glsl
void main() {
    gl_Position = Projection * ViewMatrix * ModelMatrix * vec4(position, 1.0);
    TexCoord = texCoord;
}
```

#### basic.frag
In the fragment shader I get `TexCoord` from `basic.vert` and get the `FragColor` from 
`texture(ourTexture, TexCoord)`, `ourTexture` is get from `uniform sampler2D ourTexture`
, the following code is used:

```glsl
vec4 texColor = texture(ourTexture, TexCoord);
// 將 alpha 設置為 1.0 以使背景完全透明
FragColor = vec4(texColor.rgb, 1.0);
```

#### basic.cpp
In this part I just Try to set the uniform value and make sure VBO's data can be access correctlly.
And I used three VBO buffer therefore my VBO stride is not `8 * sizeof(GLfloat)` but `3 * sizeof(GLfloat)` .
Which is  different way compared to [https://learnopengl.com/Getting-started/Textures](https://learnopengl.com/Getting-started/Textures).
Then the more detailis mentioned in the comment.
```cpp
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
  // Generate and bind vertex array objects (VAO) for each model
  glGenVertexArrays(num_model, VAO);

  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[3];
    // Generate 3 VBOs to store position, normal, and texcoord respectively.
    // This approach differs from https://learnopengl.com/Getting-started/Textures
    glGenBuffers(3, VBO);

    // VBO for positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // VBO for normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VBO for texcoords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(GLfloat), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(2);
  }

  return programId != 0;
}

void BasicProgram::doMainLoop() {
  // Render objects with shader
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];

    // Pass projection matrix to shader
    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    // Pass view matrix to shader
    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    // Pass model matrix to shader
    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    // Activate texture unit and bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);

    // Pass texture unit to shader
    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);

    // Draw the object
    glDrawArrays(model->drawMode, 0, model->numVertex);

    // Unbind the vertex array after drawing
    glBindVertexArray(0);
  }
  glUseProgram(0);
}
```
### Task#3 Display texture plane (10%)
I manully set plane positions, normals, and texcoords.
Then Adjusted positions based on the specified size and center position.

```cpp
  /* TODO#3-1: Add a plane model
   *           1. Create a model and manually set plane positions, normals, texcoords
   *           2. Add texure "../assets/models/Wood_maps/AT_Wood.jpg"
   *           3. Set m->numVertex, m->drawMode
   *           4. Add model to ctx.models
   * Note:
   *           GL_TEXTURE_WRAP is set to GL_REPEAT in createTexture, you may need to know
   *           what this means to set m->textures correctly
   */


  /* 1. Create a model and manually set plane positions, normals, texcoords */
  Model* plane = new Model();

  // Manually set plane positions, normals, and texcoords
  // Adjusted positions based on the specified size and center position
  float sizeX = 8.192f;
  float sizeY = 1.0f;
  float sizeZ = 5.12f;
  float centerX = 0.0f;
  float centerY = 0.0f;
  float centerZ = 0.0f;

  /* Set positions */ 
  plane->positions = {
      // 上平面
      -sizeX / 2 + centerX, centerY, sizeZ / 2 + centerZ,
      sizeX / 2 + centerX, centerY, sizeZ / 2 + centerZ,
      sizeX / 2 + centerX, centerY, -sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX, centerY, -sizeZ / 2 + centerZ,

 /**********************************************************************************************
  The other five faces have implement with same logic so removed them for better read experience
  **********************************************************************************************/
  };

  // Assuming the plane is facing upwards, set normals
  plane->normals = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

  /* Set texcoords */ 
  // Adjusted texcoords based on the specified texture mapping size
  float textureSizeX = 4.096f;
  float textureSizeY = 2.56f;

  //上平面
  plane->texcoords = {
      0.0f, 0.0f, sizeX / textureSizeX, 0.0f, sizeX / textureSizeX, sizeY / textureSizeY, 0.0f, sizeY / textureSizeY};
  
 /**********************************************************************************************
  The other five faces have implement with same logic so removed them for better read experience
  **********************************************************************************************/

  // Add texture "../assets/models/Wood_maps/AT_Wood.jpg"
  plane->textures.push_back(createTexture("../assets/models/Wood_maps/AT_Wood.jpg"));

  // Set m->numVertex, m->drawMode
  plane->numVertex = 24;  // 8頂點 x 3坐標
  plane->drawMode = GL_QUADS;

  // Add model to ctx.models
  ctx.models.push_back(plane);
}
```
then Put the plane into scene
```cpp
  // TODO#3-2: Put the plane into scene
  Object* planeObject = new Object(3, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096, 0, 2.56)));
  ctx.objects.push_back(planeObject);
```

### Task#4 Light shader program (three light source mixed)
#### light.vert
The `light.vert` code and explain, the logic is similar to `basic.vert`: 
```glsl
void main() {
    // Calculate world-space position, normal, and texture coordinates
    FragPos = vec3(ModelMatrix * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(ModelMatrix))) * normal;
    TexCoord = texCoord;

    // Calculate gl_Position (clip space position)
    gl_Position = Projection * ViewMatrix * ModelMatrix * vec4(position, 1.0);
}
```
#### light.frag
1. Ambient Light Calculation:

```glsl
vec3 ambient = dl.enable * dl.lightColor * material.ambient;
```
This calculates the ambient light contribution based on the presence of a directional light (dl.enable), the light color, and the ambient material property.

2. Normalization and View Direction Calculation:
```glsl
vec3 norm = normalize(Normal);
vec3 viewDir = normalize(viewPos - FragPos);
Normalizes the normal vector (Normal) and calculates the normalized view direction.
```

3. Directional Light Reflection (Diffuse and Specular):
```glsl
vec3 lightDir = normalize(-dl.direction);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = dl.enable * dl.lightColor * (diff * material.diffuse);

vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = dl.enable * dl.lightColor * (spec * material.specular);
```
Calculates the diffuse and specular reflection for a directional light.

4. Point Light Reflection (Diffuse and Specular):
```glsl
if (pl.enable == 1) {
    // ... [Point light calculations]
    diffuse += diffusePoint;
    specular += specularPoint;
}
```
If a point light is enabled (pl.enable == 1), it calculates the diffuse and specular reflections considering 
light attenuation.

5. Spotlight Reflection (Diffuse and Specular):
```glsl
if (sl.enable == 1) {
    // ... [Spotlight calculations]
    diffuse += diffuseSpot;
    specular += specularSpot;
}
```
If a spotlight is enabled (sl.enable == 1), it calculates the diffuse and specular reflections considering light 
attenuation and the spotlight's cutoff angle.

6. Final Result and Texture Mapping:
```glsl
vec3 result = ambient + diffuse + specular;
color = texture(ourTexture, TexCoord) * vec4(result, 1.0);
```
Combines ambient, diffuse, and specular components to get the final color, which is then multiplied by the texture 
color (texture(ourTexture, TexCoord)) for texture mapping.

#### light.cpp
```cpp
bool LightProgram::load() {
 /**********************************************************************************************
  Same as load() in basic.cpp so removed them for better read experience
  **********************************************************************************************/
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
```

## Problems you encountered
### GFX Glitch
I forgot to write condition code to handle `plane->drawMode = GL_QUADS`, so the bottle will be render
by `plane->drawMode = GL_TRIANGLES` then GFX Glitch issue happened on the bottle.
![Bottle_issue](https://github.com/CodeStone1125/Shader/assets/72511296/e583f155-f324-4b33-828d-aed40e58d48c)

### Texture issue
I forget I used three VBO buffer therefore my VBO stride should not be `8 * sizeof(GLfloat)` but `3 * sizeof(GLfloat)` .
Which is  different way compared to [https://learnopengl.com/Getting-started/Textures](https://learnopengl.com/Getting-started/Textures).
Therefore the Texture failed.
![Texture_issue](https://github.com/CodeStone1125/Shader/assets/72511296/9f468c33-9376-4141-ae4e-112bab6d3e96)



