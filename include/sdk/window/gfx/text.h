#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/style.h>
#include <sdk/string/wide.h>

typedef struct gfx_text_t {
  // required
  window_t* window;
  wstring_t text;
  rect_t rect;
  const gfx_style_t* style;
  const gfx_color_t* color;
} gfx_text_t;

void gfx_text_new(gfx_text_t*);
void gfx_text_free(gfx_text_t*);
void gfx_text_update(gfx_text_t*);
void gfx_text_draw(const gfx_text_t*);

#include <sdk/window/gfx/text/win32.inl.h>
#include <sdk/window/gfx/text/inl.h>