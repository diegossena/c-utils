#include <sdk/window.h>
#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/gfx/text.h>

void gfx_text_draw(const gfx_text_t* this) {
  window_t* window = this->window;
  IDWriteTextFormat* text_format = this->format->__format;
  f32 font_size = text_format->lpVtbl->GetFontSize(text_format);
  D2D1_RECT_F rect = {
    this->position.x, this->position.y,
    this->position.x + font_size * this->length,
    this->position.y + font_size
  };
  ID2D1RenderTarget_DrawText(
    window->__d2d_render_target, this->text, this->length, this->format->__format,
    &rect, (ID2D1Brush*)this->color->__brush, D2D1_DRAW_TEXT_OPTIONS_NONE,
    DWRITE_MEASURING_MODE_NATURAL
  );
}

#endif