#pragma once

#include <sdk/types.h>

typedef struct vector2d_t {
  f32 x, y;
} vector2d_t;
// Dot product
INLINE f32 vector2d_dot(const vector2d_t, const vector2d_t);
INLINE vector2d_t vector2d_mul(vector2d_t, const vector2d_t);
INLINE vector2d_t vector2d_mul_scalar(vector2d_t, f32);
INLINE vector2d_t vector2d_sub(vector2d_t, const vector2d_t);
INLINE vector2d_t vector2d_div(vector2d_t, const vector2d_t);
INLINE f32 vector2d_distance(vector2d_t, const vector2d_t);
INLINE void vec2_normalize(vector2d_t);

#include <sdk/math/geometry_2d.inl.h>