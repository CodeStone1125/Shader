#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }
  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        Vertex per face = 3 or 4
   */
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
  }

  // Now, populate the Model's data
  for (size_t i = 0; i < positionIndices.size(); ++i) {
    int posIndex = positionIndices[i];
    int texIndex = texcoordIndices[i];
    int normIndex = normalIndices[i];

    // Extract x, y, and z components from glm::vec3 and add to the vectors
    m->positions.push_back(positions[posIndex].x);
    m->positions.push_back(positions[posIndex].y);
    m->positions.push_back(positions[posIndex].z);

    m->texcoords.push_back(texcoords[texIndex].x);
    m->texcoords.push_back(texcoords[texIndex].y);

    m->normals.push_back(normals[normIndex].x);
    m->normals.push_back(normals[normIndex].y);
    m->normals.push_back(normals[normIndex].z);
  }

  m->numVertex = static_cast<int>(m->positions.size());

  ObjFile.close();
  return m;
}