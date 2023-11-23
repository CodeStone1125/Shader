project(STB
  VERSION         2.27
  DESCRIPTION     "stb single-file public domain libraries for C/C++."
  HOMEPAGE_URL    "https://github.com/nothings/stb"
  LANGUAGES       C
)

add_library(stb INTERFACE)
# Use c11 default
target_compile_features(stb INTERFACE c_std_11)
target_include_directories(stb INTERFACE ${STB_SOURCE_DIR}/include)
