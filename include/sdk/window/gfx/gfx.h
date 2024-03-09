#pragma once

#include <sdk/window/window.h>
#include <sdk/window/gfx/text_style.h>
#include <sdk/window/gfx/rect.h>

#define gfx_draw_text_cwstr(this, text, rect, style) { \
  const wchar_t __text[] = text; \
  gfx_draw_text(this, text, sizeof(text) / sizeof(wchar_t) - 1, rect, style); \
}

void gfx_draw_text(window_t*, const wchar_t* text, u64 length, gfx_frect_t*, text_style_t*);