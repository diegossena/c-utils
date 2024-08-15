#ifndef SDK_MATH_H
#define SDK_MATH_H

#include <sdk/types.h>

#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) math_min(math_max(value, min), max)
#define math_abs(value) (value < 0 ? -(value) : value)

/** @brief An approximate representation of PI. */
#define MATH_PI 3.14159265358979323846
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

SDK_EXPORT f64 math_log10(f64);
SDK_EXPORT u64 math_next2pow(u64 number);
SDK_EXPORT u64 math_hash_jenkins(const byte_t* data, u64 size);
SDK_EXPORT i64 math_ceil(f64 number);
SDK_EXPORT f64 math_pow(f64 base, f64 exponent);
SDK_EXPORT f64 math_floor(f64 number);
SDK_EXPORT f64 math_floor_f64(f64 number, u8 places);

#endif