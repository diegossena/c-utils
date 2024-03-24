#pragma once

#include <sdk/window.h>
#include <sdk/window/gfx/font.h>

typedef struct gfx_style_t gfx_style_t;

typedef struct gfx_style_props_t {
  // required
  const window_t* window;
  const wchar_t* family;
  f32 size;
  font_weight_t weight;
  // optional
  font_style_t style;
} gfx_style_props_t;

void gfx_style_new(gfx_style_t* this, gfx_style_props_t);
void gfx_style_free(gfx_style_t*);

#include <sdk/window/gfx/style/win32.inl.h>