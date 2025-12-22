#include <sdk/time.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>

f32 _time_clock_frequency;

void time_ticks_startup() {
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  _time_clock_frequency = frequency.QuadPart;
}

f32 time_ticks() {
  LARGE_INTEGER count;
  QueryPerformanceCounter(&count);
  return (f32)count.QuadPart / _time_clock_frequency;
}

#endif