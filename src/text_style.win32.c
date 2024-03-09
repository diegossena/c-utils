#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/gfx.text_style.win32.h"
#include "internal/window.win32.h"
#include "internal/memory.h"

text_style_t* text_style_new(text_style_properties_t* props, window_t* window) {
  text_style_t* this = memory_alloc0(sizeof(text_style_t));
  // CreateTextFormat
  DWRITE_FONT_WEIGHT font_weight;
  DWRITE_FONT_STYLE font_style;
  switch (props->weight) {
    case FONT_WEIGHT_NORMAL:
      font_weight = DWRITE_FONT_WEIGHT_NORMAL;
      break;
    case FONT_WEIGHT_BOLD:
      font_weight = DWRITE_FONT_WEIGHT_BOLD;
      break;
  }
  switch (props->style) {
    case FONT_STYLE_NORMAL:
      font_style = DWRITE_FONT_STYLE_NORMAL;
      break;
  }
  IDWriteFactory_CreateTextFormat(
    window->d2_write_factory, props->family, null, font_weight,
    font_style, DWRITE_FONT_STRETCH_NORMAL, props->size, L"en-US",
    &this->format
  );
  // CreateSolidColorBrush
  D2D1_COLOR_F color = { 0.f, 0.f, 0.f, 1.f };
  ID2D1RenderTarget_CreateSolidColorBrush(
    window->d2_render_target, &color, null, (ID2D1SolidColorBrush**)&this->brush
  );

  return this;
}
void text_style_free(text_style_t* this) {
  IDWriteTextFormat_Release(this->format);
  ID2D1Brush_Release(this->brush);
  memory_free(this);
}

#endif