#ifndef TIMER_H
#define TIMER_H

#include <sdk/types.h>

typedef void* timer_t;

export timer_t* timer_new(function_t callback, void* context, u32 timeout, u32 delay);
export void timer_set(timer_t* this, u32 timeout, u32 delay);
export void timer_clear(timer_t* this);

#endif