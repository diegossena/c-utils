#ifdef SDK_PROCESS_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>

void process_execpath(string_t* this) {
  __string_mutate(this, MAX_PATH);
  u32 length = GetModuleFileNameA(null, this->__data, MAX_PATH);
  if (length > 0) {
    this->__length = length;
  } else {
    error("GetModuleFileNameA", ERR_UNKNOWN);
    string_clear(this);
  }
}

#endif
#endif