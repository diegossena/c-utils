#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/types.h"
#include "sdk/path.h"
#include "internal/string.h"
#include "sdk/error.h"
#include "internal/memory.h"

#include <fileapi.h>
#include <shlwapi.h>

const char path_sep = '\\';

void path_resolve(string_t* this) {
  char buffer[MAX_PATH] = "";
  u32 length = GetFullPathNameA(this->data, MAX_PATH, buffer, NULL);
  if (length) {
    u64 capacity = length + 1;
    this->data = memory_realloc(this->data, capacity);
    this->length = length;
    memory_copy(this->data, buffer, capacity);
  } else {
    error("GetFullPathNameA", ERR_UNKNOWN);
  }
}

#endif