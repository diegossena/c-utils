#include <sdk/thread.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

u8 os_cpu_count() {
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  return system_info.dwNumberOfProcessors;
}

#endif