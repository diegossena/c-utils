#pragma once

#include <sdk/window.h>
#include <sdk/math/geometry_2d.h>

typedef struct bitmap_t bitmap_t;

typedef struct gfx_bitmap_t {
  const window_t* window;
  rect_t rect;
  bitmap_t* image;
} gfx_bitmap_t;

void gfx_bitmap_new(bitmap_t*, const wchar_t* path, const window_t*);
void gfx_bitmap_free(bitmap_t*);

void gfx_bitmap_draw(const gfx_bitmap_t*);

#include <sdk/window/gfx/bitmap/win32.inl.h>