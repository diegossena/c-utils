#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/process.h"
#include <windows.h>

string_t* process_execpath() {
  char buffer[MAX_PATH] = {};
  u32 length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
  string_t* result = string_new();
  if (length) {
    string_append_str(result, buffer, length);
  } else {
    error("GetModuleFileNameA", ERR_UNKNOWN);
  }
  return result;
}

#endif