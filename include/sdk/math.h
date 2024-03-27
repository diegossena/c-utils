#ifndef SDK_MATH_H
#define SDK_MATH_H

#include <sdk/types.h>
#include <sdk/math.h>

#define math_min(a, b) (b < a ? b : a)
#define math_max(a, b) (a < b ? b : a)
#define math_clamp(value, min, max) (math_min(math_max(value, min), max))
#define math_abs(value) (value < 0 ? -(value) : value)

#endif