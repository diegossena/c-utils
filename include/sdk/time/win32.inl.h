#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/time.h>

f64 __time_clock_frequency;
LARGE_INTEGER __time_start;

f64 time_absolute() {
  if (!__time_clock_frequency) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    __time_clock_frequency = 1. / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&__time_start);
  }
  LARGE_INTEGER now_time;
  QueryPerformanceCounter(&now_time);
  return (f64)now_time.QuadPart * __time_clock_frequency;
}
void sleep(u64 ms) { Sleep(ms); }

#endif