#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/types.h"
// path_absolute
#include "sdk/path.h"
// error
#include "sdk/error.h" 
#include <fileapi.h>
#include <shlwapi.h>

const char path_sep = '\\';

string_t* path_resolve(const char* path) {
  char buffer[MAX_PATH];
  u32 length = GetFullPathNameA(path, MAX_PATH, buffer, NULL);
  string_t* result = string_new();
  if (length) {
    string_append_str(result, buffer, length);
  } else {
    error("GetFullPathNameA", ERR_UNKNOWN);
  }
  return result;
}

#endif