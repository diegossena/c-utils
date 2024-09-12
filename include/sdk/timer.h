#ifndef SDK_TIMER_H
#define SDK_TIMER_H

#include <sdk/types.h>

typedef void* timer_t;

SDK_EXPORT timer_t* timer_new(function_t callback, void* context, u32 timeout, u32 delay);
SDK_EXPORT void timer_set(timer_t* this, u32 timeout, u32 delay);
SDK_EXPORT void timer_clear(timer_t* this);

#endif