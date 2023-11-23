#include "opengl_context.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

GLFWwindow* OpenGLContext::window = nullptr;
int OpenGLContext::refresh_rate = 60;
int OpenGLContext::major_version = 4;
int OpenGLContext::minor_version = 1;
int OpenGLContext::profile = GLFW_OPENGL_COMPAT_PROFILE;
int OpenGLContext::framebuffer_width = 1280;
int OpenGLContext::framebuffer_height = 720;

namespace {
void printSourceEnum(GLenum source) {
  std::cerr << "Source  : ";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cerr << "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cerr << "Window system";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cerr << "Shader compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cerr << "Third party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cerr << "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      [[fallthrough]];
    default:
      std::cerr << "Other";
      break;
  }
  std::cerr << std::endl;
}

void printTypeEnum(GLenum type) {
  std::cerr << "Type    : ";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cerr << "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cerr << "Deprecated behavior";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cerr << "Undefined behavior";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cerr << "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cerr << "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cerr << "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cerr << "Push group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cerr << "Pop group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      [[fallthrough]];
    default:
      std::cerr << "Other";
      break;
  }
  std::cerr << std::endl;
}

void printSeverityEnum(GLenum severity) {
  std::cerr << "Severity: ";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cerr << "High";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cerr << "Medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cerr << "Low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      [[fallthrough]];
    default:
      std::cerr << "Notification";
      break;
  }
  std::cerr << std::endl;
}

void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message,
                              const void*) {
  if (id == 131169 ||  // Allocate framebuffer
      id == 131185 ||  // Allocate buffer
      id == 131218 ||  // Shader recompile
      id == 131204 ||  // Texture no base level
      id == 13         // GL_LIGHTH is deprecated in open GL 3 (deprecated fixed function lights pipleine)
  )
    return;
  std::cerr << std::endl << "Id: " << id << " Message : " << message << std::endl;
  printSeverityEnum(severity);
  printSourceEnum(source);
  printTypeEnum(type);
}
}  // namespace

OpenGLContext::OpenGLContext() {
  // Initialize GLFW
  if (glfwInit() == GLFW_FALSE) {
    THROW_EXCEPTION(std::runtime_error, "Failed to initialize GLFW!");
  }
  // Setup context property
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
  if (OpenGLContext::profile == GLFW_OPENGL_CORE_PROFILE) {
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  }
  glfwWindowHint(GLFW_OPENGL_PROFILE, OpenGLContext::profile);
#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
  // Create OpenGL context
  window = glfwCreateWindow(1280, 720, "Hello World!", nullptr, nullptr);
  if (window == nullptr) {
    // Fallback to 3.3 first, then throw exception
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    OpenGLContext::major_version = OpenGLContext::minor_version = 3;
    window = glfwCreateWindow(1280, 720, "Hello World!", nullptr, nullptr);
    if (window == nullptr) THROW_EXCEPTION(std::runtime_error, "Failed to create OpenGL context!");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  // Load OpenGL function pointers
#ifdef GLAD_OPTION_GL_ON_DEMAND
  // Lazy loading
  gladSetGLOnDemandLoader(glfwGetProcAddress);
#else
  if (!gladLoadGL(glfwGetProcAddress)) {
    THROW_EXCEPTION(std::runtime_error, "Failed to load OpenGL!");
  }
#endif
  // For high dpi monitors like Retina display, we need to recalculate
  // framebuffer size
  glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
  glViewport(0, 0, framebuffer_width, framebuffer_height);
  // OK, everything works fine
  // ----------------------------------------------------------
  // Enable some OpenGL feature
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0, 1);
}

OpenGLContext::~OpenGLContext() {
  if (window != nullptr) glfwDestroyWindow(window);
  glfwTerminate();
}

void OpenGLContext::createContext(int GLversion, int profile) {
  // We should only initialize once
  if (window == nullptr) {
    OpenGLContext::major_version = GLversion / 10;
    OpenGLContext::minor_version = GLversion % 10;
    if (GLversion < 32)
      OpenGLContext::profile = GLFW_OPENGL_ANY_PROFILE;
    else
      OpenGLContext::profile = profile;
  }
  static OpenGLContext context;
}

void OpenGLContext::printSystemInfo() {
  GLFWmonitor* moniter = glfwGetPrimaryMonitor();
  const GLFWvidmode* vidMode = glfwGetVideoMode(moniter);
  if (vidMode == nullptr) {
    std::cerr << "Unable to get video mode of monitor." << std::endl;
    return;
  }
  OpenGLContext::refresh_rate = vidMode->refreshRate;

  std::cout << std::left << std::setw(26) << "Current OpenGL renderer"
            << ": " << glGetString(GL_RENDERER) << std::endl;
  std::cout << std::left << std::setw(26) << "Current OpenGL context"
            << ": " << glGetString(GL_VERSION) << std::endl;
  std::cout << std::left << std::setw(26) << "Moniter refresh rate"
            << ": " << refresh_rate << " Hz" << std::endl;
}

void OpenGLContext::framebufferResizeCallback(GLFWwindow*, int width, int height) {
  framebuffer_width = width;
  framebuffer_height = height;
  glViewport(0, 0, width, height);
}

void OpenGLContext::enableDebugCallback() {
  int flags = 0;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    if (glDebugMessageCallback != nullptr) {
      std::cout << "Debug context enabled, it may hurt performance." << std::endl;
      std::cout << "Build in release mode to disable debugging." << std::endl;
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(errorCallback, nullptr);
    } else {
      std::cout << "Your system does not support debug output." << std::endl;
      std::cout << "Your can manually use glGetError to debug." << std::endl;
    }
  } else {
    std::cout << "You should build with debug mode to enable this feature." << std::endl;
  }
}
