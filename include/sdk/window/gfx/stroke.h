#pragma once

typedef struct gfx_stroke_props_t {
  border_style_t style;
} gfx_stroke_props_t;

typedef struct gfx_stroke_t gfx_stroke_t;

void gfx_stroke_new(gfx_stroke_t*, window_t*, gfx_stroke_props_t);
void gfx_stroke_free(gfx_stroke_t*);

#include <sdk/window/gfx/stroke/win32.inl.h>