#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/path.h>

static const char path_sep = '\\';

void path_resolve(string_t* this) {
  char buffer[MAX_PATH] = "";
  u32 length = GetFullPathNameA(this->__data, MAX_PATH, buffer, NULL);
  if (length) {
    u64 capacity = length + 1;
    __string_mutate(this, MAX_PATH);
    this->__length = length;
    memory_copy(this->__data, buffer, capacity);
  } else {
    error("GetFullPathNameA", ERR_UNKNOWN);
  }
}

#endif