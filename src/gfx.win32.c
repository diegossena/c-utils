#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/window/gfx.h"
#include "internal/window.win32.h"

#define rect_calc(gfx_rect) {\
  gfx_rect.left, gfx_rect.top, \
  gfx_rect.width ? gfx_rect.left + gfx_rect.width : gfx_rect.right, \
  gfx_rect.height ? gfx_rect.top + gfx_rect.height : gfx_rect.bottom \
}

void gfx_draw_text(window_t* this, const wchar_t* text, u64 length, text_props_t* props) {
  IDWriteTextFormat* text_format;
  ID2D1SolidColorBrush* brush;
  D2D1_RECT_F rect = rect_calc(props->rect);
  // text_format
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
    this->d2_write_factory, props->family, null, font_weight,
    font_style, DWRITE_FONT_STRETCH_NORMAL, props->size, L"en-US",
    &text_format
  );
  // CreateColorBrush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, (D2D1_COLOR_F*)&props->color, null, (ID2D1SolidColorBrush**)&brush
  );
  // render
  ID2D1RenderTarget_DrawText(
    this->d2_render_target, text, length, text_format, &rect, (ID2D1Brush*)brush,
    D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL
  );
  // free resources
  IDWriteTextFormat_Release(text_format);
  ID2D1SolidColorBrush_Release(brush);
}
void gfx_draw_rect(window_t* this, rect_props_t* props) {
  ID2D1SolidColorBrush* brush;
  ID2D1StrokeStyle* stroke_style;
  float stroke_width = 2.f;
  D2D1_RECT_F d2_rect = rect_calc(props->rect);
  // CreateSolidColorBrush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, (D2D1_COLOR_F*)&props->color, null, &brush
  );
  // CreateStrokeStyle
  D2D1_STROKE_STYLE_PROPERTIES stroke_properties;
  stroke_properties.startCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.endCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.dashCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.lineJoin = D2D1_LINE_JOIN_ROUND;
  stroke_properties.miterLimit = 10.0f;
  stroke_properties.dashStyle = D2D1_DASH_STYLE_SOLID;
  stroke_properties.dashOffset = 0.0f;
  ID2D1Factory_CreateStrokeStyle(
    this->d2_factory, &stroke_properties, null, null, &stroke_style
  );
  // Draw
  ID2D1RenderTarget_DrawRectangle(
    this->d2_render_target, &d2_rect, (ID2D1Brush*)brush, stroke_width, null
  );
  // ID2D1RenderTarget_DrawRectangle(this->d2_render_target);
  // free resources
  ID2D1SolidColorBrush_Release(brush);
  ID2D1StrokeStyle_Release(stroke_style);
}
// void gfx_draw_ellipse(window_t* this, gfx_frect_t* rect) {

// }
#endif