#pragma once

#include <sdk/sdk.h>

f32 delta_time;

f64 __time_last_update;

void time_start() {
  __time_last_update = time_absolute();
}

void time_update() {
  f64 now = time_absolute();
  delta_time = now - __time_last_update;
  __time_last_update = now;
}