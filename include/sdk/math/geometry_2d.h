#ifndef SDK_MATH_GEOMETRY_2D_H
#define SDK_MATH_GEOMETRY_2D_H

#include <sdk/types.h>

#include <sdk/math.h>

typedef struct vector2d_t {
  f32 x, y;
} vector2d_t;

typedef struct rect_t {
  /**
   * top left corner [0, 0]
   * bottom right corner [window.height, window.width]
   */
  vector2d_t left_top, right_bottom;
} rect_t;

typedef struct overlap_2d_t {
  vector2d_t axis;
  f32 distance;
} overlap_2d_t;

#endif