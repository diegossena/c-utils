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
#include <shlwapi.h>

const char path_sep = '\\';

string_t* path_absolute(const char* path) {
  char buffer[MAX_PATH];
  u32 length = GetFullPathNameA(path, MAX_PATH, buffer, NULL);
  string_t* result = string_new();
  if (length) {
    string_append_str(result, buffer, length);
  } else {
    error("GetFullPathNameA", length);
  }
  return result;
}
string_t* path_join(const char* path1, const char* path2) {
  char buffer[MAX_PATH];
  u32 length = PathCombineA(buffer, path1, path2);
  string_t* result = string_new();
  if (length) {
    string_append_str(result, buffer, length);
  } else {
    error("GetFullPathNameA", length);
  }
  return result;
}

#endif