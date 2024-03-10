#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/time.h"
#include <windows.h>

f64 clock_frequency;
LARGE_INTEGER start_time;

f64 time_absolute() {
  if (!clock_frequency) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1. / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
  }
  LARGE_INTEGER now_time;
  QueryPerformanceCounter(&now_time);
  return (f64)now_time.QuadPart * clock_frequency;
}
void sleep(u64 ms) { Sleep(ms); }

#endif