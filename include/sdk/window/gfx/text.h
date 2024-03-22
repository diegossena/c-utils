#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/style.h>

#define gfx_text_cwstr(cwstr) \
  .text=cwstr, .length=sizeof(cwstr) / sizeof(wchar_t) - 1

typedef struct gfx_text_t {
  // required
  const window_t* window;
  const wchar_t* text;
  u64 length;
  rect_t rect;
  const gfx_style_t* style;
  const gfx_color_t* color;
} gfx_text_t;

void gfx_text_adjust(gfx_text_t*);
void gfx_text_draw(const gfx_text_t*);

#include <sdk/window/gfx/text/win32.inl.h>