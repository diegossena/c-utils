#ifndef GFX_RECT_H
#define GFX_RECT_H

#include <sdk/types.h>
#include <sdk/gfx.color.h>
#include <sdk/gfx.stroke.h>

typedef struct gfx_rect_t {
  f32 rect[4]; // [left, top, right, bottom]
  gfx_color_t* color;
  f32 border_width;
  f32 border_radius;
} gfx_rect_t;

export void gfx_fill_rect(f32 rect[4], gfx_color_t* color);
export void gfx_draw_rect(f32 rect[4], gfx_color_t* color, f32 width);

#endif