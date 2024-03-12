#include <sdk/platform/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/window.h>
#include <sdk/window/gfx.h>

typedef struct bitmap_t {
  ID2D1Bitmap* bitmap;
} bitmap_t;

void gfx_draw_text(window_t* this, const wchar_t* text, u64 length, text_props_t* props) {
  IDWriteTextFormat* text_format;
  ID2D1SolidColorBrush* brush;
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
  // brush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, (D2D1_COLOR_F*)&props->color, null, (ID2D1SolidColorBrush**)&brush
  );
  // draw
  ID2D1RenderTarget_DrawText(
    this->d2_render_target, text, length, text_format, (D2D1_RECT_F*)&props->rect, (ID2D1Brush*)brush,
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
  // brush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, (D2D1_COLOR_F*)&props->color, null, &brush
  );
  // stroke
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
  // draw
  ID2D1RenderTarget_FillRectangle(
    this->d2_render_target, (D2D1_RECT_F*)&props->rect, (ID2D1Brush*)brush
  );
  // ID2D1RenderTarget_DrawRectangle(
  //   this->d2_render_target, &rect, (ID2D1Brush*)brush, stroke_width,
  //   stroke_style
  // );
  // free resources
  ID2D1SolidColorBrush_Release(brush);
  ID2D1StrokeStyle_Release(stroke_style);
}
void gfx_draw_ellipse(window_t* this, ellipse_props_t* props) {
  ID2D1SolidColorBrush* brush;
  ID2D1StrokeStyle* stroke_style;
  float stroke_width = 2.f;
  // brush
  ID2D1RenderTarget_CreateSolidColorBrush(
    this->d2_render_target, (D2D1_COLOR_F*)&props->color, null, &brush
  );
  // stroke
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
  // draw
  ID2D1RenderTarget_DrawEllipse(
    this->d2_render_target, (D2D1_ELLIPSE*)props, (ID2D1Brush*)brush,
    stroke_width, stroke_style
  );
  // free resources
  ID2D1SolidColorBrush_Release(brush);
  ID2D1StrokeStyle_Release(stroke_style);
}
void gfx_bitmap_constructor(bitmap_t* this, window_t* window, const wchar_t* path) {
  HRESULT result;
  IWICImagingFactory* wic_factory;
  IWICBitmapDecoder* decoder;
  IWICBitmapFrameDecode* frame_decode;
  IWICFormatConverter* converter;

  result = CoInitialize(NULL);
  if (FAILED(result)) {
    error("CoInitialize", result);
    return;
  }
  CLSID clsid = CLSID_WICImagingFactory;
  IID iid = IID_IWICImagingFactory;
  result = CoCreateInstance(
    &clsid, null, CLSCTX_INPROC_SERVER,
    &iid, (void**)&wic_factory
  );
  if (FAILED(result)) {
    error("CoCreateInstance", result);
    goto wic_factory_free;
  }
  result = wic_factory->lpVtbl->CreateDecoderFromFilename(
    wic_factory, path, NULL, GENERIC_READ,
    WICDecodeMetadataCacheOnDemand, &decoder
  );
  if (FAILED(result)) {
    error("CreateDecoderFromFilename", result);
    goto decoder_free;
  }
  result = decoder->lpVtbl->GetFrame(decoder, 0, &frame_decode);
  if (FAILED(result)) {
    error("GetFrame", result);
    goto decoder_free;
  }
  result = wic_factory->lpVtbl->CreateFormatConverter(wic_factory, &converter);
  if (FAILED(result)) {
    error("CreateFormatConverter", result);
    goto frame_decode_free;
  }
  result = converter->lpVtbl->Initialize(
    converter, (IWICBitmapSource*)frame_decode, &GUID_WICPixelFormat32bppPBGRA,
    WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeMedianCut
  );
  if (FAILED(result)) {
    error("IWICFormatConverter::Initialize", result);
    goto frame_decode_free;
  }
  result = window->d2_render_target->lpVtbl->CreateBitmapFromWicBitmap(
    window->d2_render_target, (IWICBitmapSource*)converter, NULL, &this->bitmap
  );
  if (FAILED(result)) {
    error("CreateBitmapFromWicBitmap", result);
  }
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
}
void gfx_bitmap_deconstructor(bitmap_t* this) {
  ID2D1Bitmap_Release((ID2D1Bitmap*)this);
}
void gfx_draw_bitmap(window_t* this, bitmap_props_t* props) {
  // draw
  (this->d2_render_target)->lpVtbl->DrawBitmap(
    this->d2_render_target, (ID2D1Bitmap*)props->image, (D2D1_RECT_F*)&props->rect, 1.f,
    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, null
  );
}

#endif