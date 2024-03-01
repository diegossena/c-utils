#pragma once

#include <sdk/types.h>
#include <sdk/console.h>

// #if _MSC_VER
// #include <_mingw.h>
// #define debugBreak() __debugbreak()
// #else // _MSC_VER
// #define debugBreak() __builtin_trap()
// #endif //_MSC_VER

#define assert(value) { \
  if(value) { \
  } else { \
    console_error_cstr("AssertionError: " #value  " ("__FILE__ ":" STRINGIZE(__LINE__)")"); \
  } \
}
