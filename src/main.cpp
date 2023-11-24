#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "context.h"
#include "gl_helper.h"
#include "model.h"
#include "opengl_context.h"
#include "program.h"
#include "utils.h"

void initOpenGL();
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int, int action, int);

Context ctx;

Material mFlatwhite;
Material mShinyred;
Material mClearblue;

void loadMaterial() {
  mFlatwhite.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
  mFlatwhite.specular = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.shininess = 10;

  mShinyred.ambient = glm::vec3(0.1985f, 0.0000f, 0.0000f);
  mShinyred.diffuse = glm::vec3(0.5921f, 0.0167f, 0.0000f);
  mShinyred.specular = glm::vec3(0.5973f, 0.2083f, 0.2083f);
  mShinyred.shininess = 100.0f;

  mClearblue.ambient = glm::vec3(0.0394f, 0.0394f, 0.3300f);
  mClearblue.diffuse = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.specular = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.shininess = 10;
}

void loadPrograms() {
  ctx.programs.push_back(new ExampleProgram(&ctx));
  ctx.programs.push_back(new BasicProgram(&ctx));
  ctx.programs.push_back(new LightProgram(&ctx));

  for (auto iter = ctx.programs.begin(); iter != ctx.programs.end(); iter++) {
    if (!(*iter)->load()) {
      std::cout << "Load program fail, force terminate" << std::endl;
      exit(1);
    }
  }
  glUseProgram(0);
}

void loadModels() {
  // TODO#1-1 Commnet out example object and uncomment models

  Model* m = Model::fromObjectFile("../assets/models/cube/cube.obj");
  m->textures.push_back(createTexture("../assets/models/cube/texture.bmp"));
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
  ctx.models.push_back(m);

  m = Model::fromObjectFile("../assets/models/Mugs/Models/Mug_obj3.obj");
  m->textures.push_back(createTexture("../assets/models/Mugs/Textures/Mug_C.png"));
  m->textures.push_back(createTexture("../assets/models/Mugs/Textures/Mug_T.png"));
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(6.0f, 6.0f, 6.0f));
  ctx.models.push_back(m);

  m = Model::fromObjectFile("../assets/models/bottle/bottle.obj");
  m->textures.push_back(createTexture("../assets/models/bottle/bottle.jpg"));
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  m->drawMode = GL_QUADS;
  ctx.models.push_back(m);


  /* TODO#3-1: Add a plane model
   *           1. Create a model and manually set plane positions, normals, texcoords
   *           2. Add texure "../assets/models/Wood_maps/AT_Wood.jpg"
   *           3. Set m->numVertex, m->drawMode
   *           4. Add model to ctx.models
   * Note:
   *           GL_TEXTURE_WRAP is set to GL_REPEAT in createTexture, you may need to know
   *           what this means to set m->textures correctly
   */
  Model* plane = new Model();

  // Manually set plane positions, normals, and texcoords
  // Adjusted positions based on the specified size and center position
  float sizeX = 8.192f;
  float sizeY = 1.0f;
  float sizeZ = 5.12f;
  float centerX = 0.0f;
  float centerY = 0.0f;
  float centerZ = 0.0f;

  plane->positions = {
      // 上平面
      -sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,

      // 下平面
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,

      // 前(以DEMO照片為基準)
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,

      // 右
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,

      // 後
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,
      sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,

      // 左
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      -sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      -sizeY / 2 + centerY,
      sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      centerY,
      sizeZ / 2 + centerZ,
      -sizeX / 2 + centerX,
      centerY,
      -sizeZ / 2 + centerZ,
  };

  // Assuming the plane is facing upwards, set normals
  plane->normals = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

  // Set texcoords
  // Adjusted texcoords based on the specified texture mapping size
  float textureSizeX = 4.096f;
  float textureSizeY = 2.56f;

  plane->texcoords = {
      0.0f, 0.0f, sizeX / textureSizeX, 0.0f, sizeX / textureSizeX, sizeY / textureSizeY, 0.0f, sizeY / textureSizeY};

  // Add texture "../assets/models/Wood_maps/AT_Wood.jpg"
  plane->textures.push_back(createTexture("../assets/models/Wood_maps/AT_Wood.jpg"));

  // Set m->numVertex, m->drawMode
  plane->numVertex = 24;  // 8頂點 x 3坐標
  plane->drawMode = GL_QUADS;

  // Add model to ctx.models
  ctx.models.push_back(plane);
}

void setupObjects() {
  // TODO#1-2 Comment out example object and uncomment model object
  // ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0 ,0))));

  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.5, 0.2, 2))));
  (*ctx.objects.rbegin())->material = mFlatwhite;
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(2.5, 0.2, 2))));
  (*ctx.objects.rbegin())->material = mShinyred;
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.5, 0.2, 2))));
  (*ctx.objects.rbegin())->material = mClearblue;

  ctx.objects.push_back(new Object(1, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3, 0.3, 3))));
  ctx.objects.push_back(new Object(1, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4, 0.3, 3))));
  (*ctx.objects.rbegin())->textureIndex = 1;

  ctx.objects.push_back(new Object(2, glm::translate(glm::identity<glm::mat4>(), glm::vec3(5, 0, 3))));

  // TODO#3-2: Put the plane into scene
  Object* planeObject = new Object(3, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096, 0, 2.56)));
  ctx.objects.push_back(planeObject);
  //(*ctx.objects.rbegin())->textureIndex = 2;  // Set the correct texture index for the plane model

}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW2 - `your student id`"
   *         Ex. HW2 - 312550000
   */
  glfwSetWindowTitle(window, "HW2 - 312551132");

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbacks use
  glfwSetWindowUserPointer(window, &camera);
  ctx.camera = &camera;
  ctx.window = window;

  loadMaterial();
  loadModels();
  loadPrograms();
  setupObjects();

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);

    ctx.spotLightDirection = glm::normalize(glm::vec3(3, 0.3, 3) - ctx.spotLightPosition);
    ctx.pointLightPosition = glm::vec3(6 * glm::cos(glm::radians(ctx._pointLightPosisionDegree)), 3.0f,
                                       6 * glm::sin(glm::radians(ctx._pointLightPosisionDegree)));
    ctx.programs[ctx.currentProgram]->doMainLoop();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_K:
        ctx._pointLightPosisionDegree += 1.0f;
        break;
      case GLFW_KEY_L:
        ctx._pointLightPosisionDegree -= 1.0f;
        break;
      case GLFW_KEY_I:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x - 0.1f, -2.0f, 8.0f);
        break;
      case GLFW_KEY_O:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x + 0.1f, -2.0f, 8.0f);
        break;

      default:
        break;
    }
    return;
  }
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F9: {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
          // Show the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
          // Hide the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
      }
      case GLFW_KEY_1:
        ctx.currentProgram = 0;
        break;
      case GLFW_KEY_2:
        ctx.currentProgram = 1;
        break;
      case GLFW_KEY_3:
        ctx.currentProgram = 2;
        break;
      case GLFW_KEY_4:
        ctx.directionLightEnable = !ctx.directionLightEnable;
        break;
      case GLFW_KEY_5:
        ctx.pointLightEnable = !ctx.pointLightEnable;
        break;
      case GLFW_KEY_6:
        ctx.spotLightEnable = !ctx.spotLightEnable;
        break;
      case GLFW_KEY_H:
        ctx.pointLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_J:
        ctx.pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
      case GLFW_KEY_Y:
        ctx.spotLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_U:
        ctx.spotLightColor = glm::vec3(0.0f, 0.8f, 0.0f);
        break;
      default:
        break;
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}