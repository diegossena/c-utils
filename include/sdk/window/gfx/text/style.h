#pragma once

#include <sdk/window.h>
#include <sdk/window/gfx/font.h>

typedef struct gfx_text_style_t gfx_text_style_t;

typedef struct text_style_props_t {
  // required
  const window_t* window;
  const wchar_t* family;
  f32 size;
  font_weight_t weight;
  // optional
  font_style_t style;
} text_style_props_t;

void gfx_text_style_new(gfx_text_style_t* this, text_style_props_t);
void gfx_text_style_free(gfx_text_style_t*);

#include <sdk/window/gfx/text/style/win32.inl.h>