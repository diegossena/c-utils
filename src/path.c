// path_join
#include "sdk/path.h"
#include "internal/memory.h"

string_t* path_join(const char* path1, u64 length1, const char* path2, u64 length2) {
  char buffer[PATH_MAX] = "";
  u64 length = 0;
  char ch = path1[length1 - 1];

  memory_copy(buffer, path1, length1);
  length += length1;

  if (ch != '\\' && ch != '/') {
    buffer[length++] = path_sep;
  }

  memory_copy(buffer + length, path2, length2);
  length += length2;

  string_t* str = string_new();
  string_append_str(str, buffer, length);
  return str;
}