#pragma once
#include <sdk/types.h>

// macro functions to work with any type

#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) (math_min(math_max(value, min), max))
#define math_abs(value) (value < 0 ? -(value) : value)

i64 math_ceil(f64);
i64 math_round(f64);
f64 math_floor(f64);
f64 math_ceil_f64(f64, u8 places);
f64 math_floor_f64(f64, u8 places);
f64 math_round_f64(f64, u8 places);
f64 math_pow(f64 base, f64 exponent);
u64 math_next2pow(u64 number);
u64 math_hash_jenkins(const byte* data, u64 size);

#include <sdk/math/math.inl.h>