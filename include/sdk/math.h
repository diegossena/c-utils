#ifndef MATH_H
#define MATH_H

#include <sdk/types.h>
/**
 * srand
 */
#include <stdlib.h>

/**
 * An approximate representation of PI.
 */
#define MATH_PI 3.14159265358979323846

#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) math_min(math_max(value, min), max)
#define math_abs(value) (value < 0 ? -(value) : value)

#define math_digits(number) ( \
  number > 0 \
    ? (u64)math_log10(number) + 1 \
    : number < 0 \
      ? (u64)math_log10((u64)~number + 1) + 2 \
      : 1 \
)
#define math_udigits(number) ( \
  number \
    ? (u64)math_log10(number) + 1 \
    : 1 \
)

export f64 math_log10(f64);
export u64 math_next2pow(u64 number);
export u64 math_hash_jenkins(const char* data, u64 size);
export f64 math_pow(f64 base, f64 exponent);
export f64 math_floor(f64 number);
export f64 math_ceil(f64 number);
export i32 math_random();
export i32 math_random_in_range(i32 min, i32 max);

#endif