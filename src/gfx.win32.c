#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/window/gfx/gfx.h"
#include "internal/gfx.text_style.win32.h"
#include "internal/window.win32.h"

void gfx_draw_text(window_t* this, const wchar_t* text, u64 length, gfx_frect_t* rect, text_style_t* style) {
  ID2D1RenderTarget_DrawText(
    this->d2_render_target, text, length, style->format, (D2D1_RECT_F*)rect,
    style->brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL
  );
}

#endif