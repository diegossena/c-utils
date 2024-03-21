#include <sdk/window.h>
#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/gfx/bitmap.h>

typedef struct bitmap_t {
  ID2D1Bitmap* __bitmap;
} bitmap_t;

void gfx_bitmap_new(bitmap_t* this, const wchar_t* path, const window_t* window) {
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
  result = window->__d2d_render_target->lpVtbl->CreateBitmapFromWicBitmap(
    window->__d2d_render_target, (IWICBitmapSource*)converter, NULL,
    &this->__bitmap
  );
  if (FAILED(result)) {
    error("CreateBitmapFromWicBitmap", result);
  }
  ++memory_leaks;
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
}
void gfx_bitmap_free(bitmap_t* this) {
  ID2D1Bitmap_Release(this->__bitmap);
  --memory_leaks;
}

void gfx_bitmap_draw(const gfx_bitmap_t* this) {
  ID2D1RenderTarget* render_target = this->window->__d2d_render_target;
  ID2D1RenderTarget_DrawBitmap(
    render_target, (ID2D1Bitmap*)this->image, (D2D1_RECT_F*)&this->rect, 1.f,
    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, null
  );
}

#endif