#pragma once

#include <sdk/window.h>
#include <sdk/math/geometry_2d.h>

typedef enum bitmap_extend_t {
  BITMAP_EXTEND_REPEAT,
  BITMAP_EXTEND_NO_REPEAT,
  BITMAP_EXTEND_COVER
} bitmap_extend_t;

typedef struct image_src_t image_src_t;

typedef struct gfx_image_t {
  const window_t* window;
  image_src_t* src;
  rect_t rect;
  vector2d_t position;
  size_u16_t size;
  // optional
  bitmap_extend_t extend_mode;
} gfx_image_t;

void gfx_image_new(image_src_t*, const wchar_t* path, const window_t*);
void gfx_image_free(image_src_t*);

void gfx_image_draw(const gfx_image_t*);

#include <sdk/window/gfx/image/win32.inl.h>