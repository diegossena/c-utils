#include <sdk/time.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>

f64 __clock_frequency = 0;

export f64 time_now_f64() {
  if (!__clock_frequency) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    __clock_frequency = (f64)frequency.QuadPart;
  }
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return (f64)now.QuadPart / __clock_frequency;
}

#endif