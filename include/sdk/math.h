#ifndef MATH_H
#define MATH_H

#include <sdk/types.h>
#include <sdk/time.h>
/**
 * srand
 */
#include <stdlib.h>

/**
 * An approximate representation of PI.
 */
#define MATH_PI 3.14159265358979323846

#define EPSILON .000001f

#define align(value, alignment) ((value + alignment - 1) & ~(alignment - 1))
#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) math_min(math_max(value, min), max)
#define math_abs(value) (value < 0 ? -(value) : value)

#define math_digits(number) ( \
  number > 0 \
    ? math_log10(number) + 1 \
    : number < 0 \
      ? math_log10(-number + 1) + 2 \
      : 1 \
)

inline u64 math_next2pow(u64 number);
inline f64 math_pow(f64 base, f64 exponent);
inline f64 math_floor(f64 number);
inline f32 math_floorf(f32 number);
inline f64 math_round(f64 number);
inline f32 math_roundf(f32 number);
inline f64 math_ceil(f64 number);
inline void math_random_startup();
inline i32 math_random();
inline i32 math_random_in_range(i32 min, i32 max);
inline f32 math_epsilon_roundf(f32 number);

f64 math_log10(f64);
u64 math_hash_jenkins(const char* data, u64 size);

#endif