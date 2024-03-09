#pragma once

#include <sdk/window.h>
#include <sdk/gfx/text_style.h>
#include <sdk/gfx/rect.h>

#define gfx_draw_text_cstr(this, text, rect, style) { \
  const wchar_t __text[] = text; \
  gfx_draw_text(this, text, sizeof(text) / sizeof(wchar_t) - 1, rect, style); \
}

void gfx_draw_text(window_t*, const wchar_t* text, u64 length, gfx_rect_f*, text_style_t*);