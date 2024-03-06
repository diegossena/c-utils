#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/types.h"
// path_absolute
#include "sdk/path.h"
// error
#include "sdk/error.h" 
// TODO: remove this line
#include "sdk/console.h"
#include <fileapi.h>

const char path_sep = '\\';

string_t* path_absolute(const char* path) {
  char path_buffer[MAX_PATH];
  u32 length = GetFullPathNameA(path, MAX_PATH, path_buffer, NULL);
  if (length == 0) {
    error("GetFullPathNameA", length);
    return 0;
  }
  string_t* result = string_new();
  string_append_str(result, path_buffer, length);
  return result;
}

#endif