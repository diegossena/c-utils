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

void gfx_draw_rect(window_t* this, gfx_frect_t* rect) {
  ID2D1SolidColorBrush* brush;
  ID2D1StrokeStyle* stroke_style;
  float stroke_width = 2.f;
  D2D1_COLOR_F color = { 0.f, 0.f, 0.f, 1.f };
  D2D1_RECT_F d2_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
  // CreateSolidColorBrush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, &color, null, &brush
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
  (this->d2_render_target)->lpVtbl->DrawRectangle(
    this->d2_render_target, &d2_rect, (ID2D1Brush*)brush, stroke_width,
    null
  );
  // ID2D1RenderTarget_DrawRectangle(this->d2_render_target);
  // free resources
  ID2D1SolidColorBrush_Release(brush);
  ID2D1StrokeStyle_Release(stroke_style);
}

#endif