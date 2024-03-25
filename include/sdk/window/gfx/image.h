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
  vector2d_t position;
  // optional
  image_extend_t extend_mode;
  // generated
  u16 width, height;
} gfx_image_t;

void gfx_image_src_new(gfx_image_src_t*, const wchar_t* path, const window_t*);
void gfx_image_src_free(gfx_image_src_t*);

#endif