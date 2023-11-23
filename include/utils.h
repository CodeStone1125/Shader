#pragma once
#include <stdexcept>
#include <string>

#ifdef __APPLE__
#ifndef HAS_CXX20_SUPPORT
#define HAS_CXX20_SUPPORT 0
#endif
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef DELETE_COPY
#define DELETE_COPY(ClassName)           \
  ClassName(const ClassName &) = delete; \
  ClassName &operator=(const ClassName &) = delete;
#endif

#ifndef DEFAULT_COPY
#define DEFAULT_COPY(ClassName)           \
  ClassName(const ClassName &) = default; \
  ClassName &operator=(const ClassName &) = default;
#endif

#ifndef DELETE_MOVE
#define DELETE_MOVE(ClassName)      \
  ClassName(ClassName &&) = delete; \
  ClassName &operator=(ClassName &&) = delete;
#endif

#ifndef DEFAULT_MOVE
#define DEFAULT_MOVE(ClassName)      \
  ClassName(ClassName &&) = default; \
  ClassName &operator=(ClassName &&) = default;
#endif

#ifndef MOVE_ONLY
#define MOVE_ONLY(ClassName) DELETE_COPY(ClassName) DEFAULT_MOVE(ClassName)
#endif

#ifndef THROW_EXCEPTION
#define THROW_EXCEPTION(ExceptionType, message)                                                                      \
  do {                                                                                                               \
    throw ExceptionType(std::string("[") + __FILE__ + ":" + std::to_string(__LINE__) + "] " + std::string(message)); \
  } while (false)
#endif

#ifndef HAS_CXX20_SUPPORT
#if __cplusplus >= 202002L
#define HAS_CXX20_SUPPORT 1
#include <bit>
#else
#define HAS_CXX20_SUPPORT 0
#endif  // __cplusplus >= 202002L
#endif  // HAS_CXX20_SUPPORT

// Some useful C++ 20 feature
#ifndef CONSTEXPR_VIRTUAL
#if HAS_CXX20_SUPPORT
#define CONSTEXPR_VIRTUAL constexpr
#else
#define CONSTEXPR_VIRTUAL
#endif  // HAS_CXX20_SUPPORT
#endif  // CONSTEXPR_VIRTUAL

// Some useful functions
namespace utils {

template <typename T>
constexpr inline T PI() {
  return static_cast<T>(M_PI);
}

template <typename T>
constexpr inline T PI_2() {
  return static_cast<T>(M_PI_2);
}

#if HAS_CXX20_SUPPORT
constexpr inline uint32_t log2(uint32_t n) { return std::bit_width(n) - 1; }
#else
constexpr inline uint32_t log2(uint32_t n) { return (n > 0) ? 1 + log2(n >> 1) : 0; }
#endif  // HAS_CXX20_SUPPORT
}  // namespace utils
