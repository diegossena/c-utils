#ifndef SDK_MATH_GEOMETRY_2D_H
#define SDK_MATH_GEOMETRY_2D_H

#include <sdk/types.h>

typedef struct vector2d_t {
  f32 x, y;
} vector2d_t;
// Dot product
INLINE f32 vector2d_dot(const vector2d_t, const vector2d_t);
INLINE void vector2d_sum(vector2d_t*, const vector2d_t*);
INLINE void vector2d_mul(vector2d_t*, const vector2d_t*);
INLINE void vector2d_mul_scalar(vector2d_t*, f32);
INLINE vector2d_t vector2d_sub(vector2d_t, const vector2d_t);
INLINE vector2d_t vector2d_div(vector2d_t, const vector2d_t);
INLINE f32 vector2d_distance(vector2d_t, const vector2d_t);
INLINE void vec2_normalize(vector2d_t);

typedef struct rect_t {
  /**
   * top left corner [0, 0]
   * bottom right corner [window.height, window.width]
   */
  vector2d_t left_top, right_bottom;
} rect_t;
INLINE void rect_set_width(rect_t*, f32);
INLINE void rect_set_height(rect_t*, f32);
INLINE void rect_set_size(rect_t*, f32);

typedef struct overlap_2d_t {
  vector2d_t axis;
  f32 distance;
} overlap_2d_t;
overlap_2d_t overlap_rect_rect(const rect_t*, const rect_t*);

#include <sdk/math/geometry_2d.inl.h>

#endif