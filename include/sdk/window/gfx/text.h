#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/style.h>

#define gfx_text_cwstr(cwstr) \
  .text=cwstr, .length=sizeof(cwstr) / sizeof(wchar_t) - 1

typedef struct gfx_text_t {
  const window_t* window;
  wchar_t* text;
  u64 length;
  vector2d_t position;
  gfx_color_t* color;
  gfx_style_t* format;
} gfx_text_t;

void gfx_text_draw(const gfx_text_t*);

#include <sdk/window/gfx/text/win32.inl.h>