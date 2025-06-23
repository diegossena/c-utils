#ifndef TIME_H
#define TIME_H

#include <sdk/types.h>

void time_ticks_startup();

u64 time_now();
f64 time_ticks();

#endif