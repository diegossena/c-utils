#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/style.h>

typedef struct gfx_text_t {
  wchar_t* text;
  u64 length;
  vector2d_t position;
  gfx_color_t* color;
  gfx_style_t* format;
  const window_t* window;
} gfx_text_t;

void gfx_text_draw(const gfx_text_t*);