project(GLAD
  VERSION         2
  DESCRIPTION     "Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs."
  HOMEPAGE_URL    "https://github.com/Dav1dde/glad/tree/glad"
  LANGUAGES       C
)

add_library(glad INTERFACE)
# Use c11 default
target_compile_features(glad INTERFACE c_std_11)
target_include_directories(glad INTERFACE ${GLAD_SOURCE_DIR}/include)
