#pragma once
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "utils.h"

class OpenGLContext final {
 public:
  // Not copyable
  DELETE_COPY(OpenGLContext)
  // Not movable
  DELETE_MOVE(OpenGLContext)
  /// @brief Release resources
  ~OpenGLContext();
  /**
   * @brief Create OpenGL context.
   *
   * @param GLversion Minimal version of OpenGL context, (pass 41 if you want OpenGL 4.1 context)
   * @param profile OpenGL profile, can be one of GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_ANY_PROFILE or
   * GLFW_OPENGL_COMPAT_PROFILE. Note that for GLversion < 32, you should always use GLFW_OPENGL_ANY_PROFILE
   *
   */
  static void createContext(int GLversion, int profile);
  /// @return Current window handle.
  static GLFWwindow* getWindow() { return window; }
  /// @return Refresh rate of the primary monitor.
  static int getRefreshRate() { return refresh_rate; }
  /// @return Current framebuffer width
  static int getWidth() { return framebuffer_width; }
  /// @return Current framebuffer height
  static int getHeight() { return framebuffer_height; }
  /// @return Current framebuffer aspect ratio
  static float getAspectRatio() { return static_cast<float>(framebuffer_width) / framebuffer_height; }
  /// @brief Enable OpenGL's debug callback
  static void printSystemInfo();
  /// @brief Framebuffer resize callback function
  static void framebufferResizeCallback(GLFWwindow* _window, int width, int height);
  /// @brief Enable OpenGL's debug callback, useful for debugging.
  static void enableDebugCallback();

 private:
  /// @brief Create OpenGL context, call by createContext method
  OpenGLContext();
  static int major_version, minor_version;
  static int profile;
  // Cached data
  static GLFWwindow* window;
  static int refresh_rate;
  // Current framebuffer size, in PIXEL (not screen coordinate)
  static int framebuffer_width, framebuffer_height;
};
