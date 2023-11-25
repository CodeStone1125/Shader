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
how to handle the obj file is the only thing I need to do in Task#1. I use four if-else conditions to handle v, vn,
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
### Task#3 Display texture plane (10%)
### Task#4 Light shader program (three light source mixed)
## Problems you encountered


