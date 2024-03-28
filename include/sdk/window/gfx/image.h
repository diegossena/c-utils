#ifndef SDK_WINDOW_GFX_H
#define SDK_WINDOW_GFX_H

#include <sdk/window.h>
#include <sdk/math/geometry_2d.h>

typedef enum image_extend_t {
  BITMAP_EXTEND_REPEAT,
  BITMAP_EXTEND_NO_REPEAT,
  BITMAP_EXTEND_COVER
} image_extend_t;

typedef struct gfx_image_src_t gfx_image_src_t;

typedef struct gfx_image_t {
  window_t* window;
  gfx_image_src_t* src;
  rect_t rect;
  vector2d_t src_position;
  // optional
  image_extend_t extend_mode;
  // generated
  u16 src_width, src_height;
} gfx_image_t;

#endif