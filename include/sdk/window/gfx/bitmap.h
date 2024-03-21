#pragma once

#include <sdk/window.h>
#include <sdk/math/geometry_2d.h>

typedef enum bitmap_extend_t {
  BITMAP_EXTEND_REPEAT,
  BITMAP_EXTEND_NO_REPEAT,
  BITMAP_EXTEND_COVER
} bitmap_extend_t;

typedef struct bitmap_t bitmap_t;

typedef struct gfx_bitmap_t {
  const window_t* window;
  bitmap_t* image;
  rect_t rect;
  vector2d_t position;
  size_u16_t size;
  bitmap_extend_t extend_mode;
} gfx_bitmap_t;

void gfx_bitmap_new(bitmap_t*, const wchar_t* path, const window_t*);
void gfx_bitmap_free(bitmap_t*);

void gfx_bitmap_draw(const gfx_bitmap_t*);

#include <sdk/window/gfx/bitmap/win32.inl.h>