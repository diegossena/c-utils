#include <sdk/window.h>
#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/gfx/text.h>

void gfx_text_update(gfx_text_t* this) {
  IDWriteTextFormat* text_format = this->style->__format;
  IDWriteFactory* factory = this->window->__d2d_write_factory;
  IDWriteTextLayout* text_layout;
  DWRITE_TEXT_METRICS metrics;
  IDWriteFactory_CreateTextLayout(
    factory, this->text.__data, this->text.__length, text_format, MAX_F32, MAX_F32,
    &text_layout
  );
  IDWriteTextLayout_GetMetrics(text_layout, &metrics);
  IDWriteTextLayout_Release(text_layout);
  this->rect.right_bottom.x = this->rect.left_top.x + metrics.width;
  this->rect.right_bottom.y = this->rect.left_top.y + metrics.height;
}

void gfx_text_draw(const gfx_text_t* this) {
  ID2D1RenderTarget* render_target = this->window->__d2d_render_target;
  IDWriteTextFormat* text_format = this->style->__format;
  ID2D1RenderTarget_DrawText(
    render_target, this->text.__data, this->text.__length, this->style->__format,
    (D2D1_RECT_F*)&this->rect, (ID2D1Brush*)this->color->__brush,
    D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL
  );
}

#endif