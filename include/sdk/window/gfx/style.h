#pragma once

#include <sdk/window.h>
#include <sdk/window/gfx/font.h>

typedef struct gfx_style_t gfx_style_t;

typedef struct gfx_style_props_t {
  window_t* window;
  f32 size;
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
} gfx_style_props_t;

void gfx_style_new(gfx_style_t* this, gfx_style_props_t);
void gfx_style_free(gfx_style_t*);