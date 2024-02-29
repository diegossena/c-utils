#pragma once

#include <sdk/types.h>

void _assert(const char* expression, const char* file, u64 line);

#if _MSC_VER
#include <_mingw.h>
#define debugBreak() __debugbreak()
#else // _MSC_VER
#define debugBreak() __builtin_trap()
#endif //_MSC_VER

#define assert(value) {                  \
  if(value) {                            \
  } else {                               \
    _assert(#value, __FILE__, __LINE__); \
    debugBreak();                        \
  }                                      \
}
