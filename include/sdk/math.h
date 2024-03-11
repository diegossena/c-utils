#pragma once
#include <sdk/types.h>

// macro functions to work with any type

#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) (math_min(math_max(value, min), max))
#define math_abs(value) (value < 0 ? -(value) : value)

u64 math_next2pow(u64 number);
u64 math_hash_jenkins(const byte* data, u64 size);
