#pragma once

#include <sdk/math/geometry_2d.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/color.h>

typedef struct gfx_rect_t {
  // required
  const window_t* window;
  rect_t rect;
  gfx_color_t* color;
  // optional
  gfx_stroke_t* stroke;
  f32 border_width;
  f32 border_radius;
} gfx_rect_t;

void gfx_rect_draw(const gfx_rect_t*);

#include <sdk/window/gfx/rect/win32.inl.h>