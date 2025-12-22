#ifndef TIME_H
#define TIME_H

#include <sdk/types.h>

#define for_animation(var, duration) \
  for (f32 _timer = time_ticks(), var = 0;; var = math_min(1.f, (time_ticks() - _timer) / 1.5f))

void time_ticks_startup();

u64 time_now();
f32 time_ticks();

#endif