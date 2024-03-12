#include <sdk/platform/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/path/path.h>
#include <sdk/platform/win32.h>

static const char path_sep = '\\';

void path_resolve(string_t* this) {
  char buffer[MAX_PATH] = "";
  u32 length = GetFullPathNameA(this->__data, MAX_PATH, buffer, NULL);
  if (length) {
    u64 capacity = length + 1;
    this->__data = memory_realloc(this->__data, capacity);
    this->__length = length;
    memory_copy(this->__data, buffer, capacity);
  } else {
    error("GetFullPathNameA", ERR_UNKNOWN);
  }
}

#endif