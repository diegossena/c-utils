#ifndef SDK_MATH_GEOMETRY_2D_H
#define SDK_MATH_GEOMETRY_2D_H

#include <sdk/types.h>

typedef struct vector2d_t {
  f32 x, y;
} vector2d_t;
// Dot product
f32 vector2d_dot(const vector2d_t, const vector2d_t);
vector2d_t vector2d_sum(const vector2d_t, const vector2d_t);
vector2d_t vector2d_mul(const vector2d_t, const vector2d_t);
vector2d_t vector2d_mul_scalar(const vector2d_t, f32);
vector2d_t vector2d_sub(const vector2d_t, const vector2d_t);
vector2d_t vector2d_div(const vector2d_t, const vector2d_t);
f32 vector2d_distance(const vector2d_t, const vector2d_t);
void vec2_normalize(vector2d_t);

typedef struct rect_t {
  /**
   * top left corner [0, 0]
   * bottom right corner [window.height, window.width]
   */
  vector2d_t left_top, right_bottom;
} rect_t;
void rect_set_width(rect_t*, u16);
void rect_set_height(rect_t*, u16);
void rect_set_size(rect_t*, u16);

bool rect_pointin(const rect_t*, vector2d_t);

typedef struct overlap_2d_t {
  vector2d_t axis;
  f32 distance;
} overlap_2d_t;
overlap_2d_t overlap_rect_rect(const rect_t*, const rect_t*);

#endif