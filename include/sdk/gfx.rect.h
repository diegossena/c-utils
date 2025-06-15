#ifndef GFX_RECT_H
#define GFX_RECT_H

#include <sdk/types.h>
#include <sdk/gfx.color.h>
#include <sdk/gfx.stroke.h>

#define MAX_RECT 7680.f

void gfx_fill_rect(const f32 rect[4], gfx_color_t* color);
void gfx_draw_rect(const f32 rect[4], gfx_color_t* color, f32 width, gfx_stroke_t* stroke);

#endif