#ifndef SDK_WINDOW_GFX_RECT_H
#define SDK_WINDOW_GFX_RECT_H

#include <sdk/math/geometry_2d.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/color.h>

typedef struct gfx_rect_t {
  // required
  window_t* window;
  const gfx_color_t* color;
  rect_t rect;
  // optional
  const gfx_stroke_t* stroke;
  f32 border_width;
  f32 border_radius;
} gfx_rect_t;

void gfx_rect_new(gfx_rect_t*);
void gfx_rect_free(gfx_rect_t*);
void gfx_rect_draw(const gfx_rect_t*);

#endif