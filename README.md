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
I manully set plane positions, normals, and texcoords and I did not set the center point to plane.
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

## Problems you encountered


