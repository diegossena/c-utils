#include <sdk/time.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>

f64 __time_clock_frequency = 0;

export f64 time_now_f64() {
  if (!__time_clock_frequency) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    __time_clock_frequency = frequency.QuadPart;
  }
  LARGE_INTEGER now_time;
  QueryPerformanceCounter(&now_time);
  return (f64)now_time.QuadPart / __time_clock_frequency;
}

#endif