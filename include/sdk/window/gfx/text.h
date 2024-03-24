#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/style.h>
#include <sdk/string/wide.h>

typedef struct gfx_text_t {
  // required
  window_t* window;
  rect_t rect;
  const gfx_style_t* style;
  const gfx_color_t* color;
  // props
  wstring_t text;
  // private
  event_listener_t __ondraw;
  event_listener_t __ondestroy;
} gfx_text_t;

void gfx_text_new(gfx_text_t*);
void gfx_text_free(gfx_text_t*);
void gfx_text_adjust(gfx_text_t*);

#include <sdk/window/gfx/text/win32.inl.h>
#include <sdk/window/gfx/text/inl.h>
