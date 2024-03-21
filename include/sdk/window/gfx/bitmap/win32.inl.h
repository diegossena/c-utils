#include <sdk/window.h>
#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/gfx/bitmap.h>

typedef struct bitmap_t {
  ID2D1Bitmap* __bitmap;
  const u16 width, height;
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
  frame_decode->lpVtbl->GetSize(frame_decode, (UINT*)&this->width, (UINT*)&this->height);
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
    window->__d2d_render_target, (IWICBitmapSource*)converter, null,
    &this->__bitmap
  );
  if (FAILED(result)) {
    error("CreateBitmapFromWicBitmap", result);
  }
  // D2D1_SIZE_U image_size = this->__bitmap->lpVtbl->GetPixelSize(this->__bitmap);
  // u16 image_width = image_size.width;
  // u16 image_height = image_size.height;
  // memory_copy((u16*)&this->width, &image_width, sizeof(u16));
  // memory_copy((u16*)&this->width, &image_height, sizeof(u16));
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
  D2D1_RECT_F position = {
    this->position.x, this->position.y,
    this->position.x + this->size.width, this->position.y + this->size.height,
  };
  switch (this->extend_mode) {
    case BITMAP_EXTEND_NO_REPEAT: {
      ID2D1RenderTarget_DrawBitmap(
        this->window->__d2d_render_target, (ID2D1Bitmap*)this->image->__bitmap,
        (D2D1_RECT_F*)&this->rect, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        &position
      );
    } break;
    default: {
      // repeat
      D2D1_RECT_F rect = {
        .top = this->rect.left_top.y,
        .bottom = this->rect.left_top.y + this->size.height,
      };
      while (true) {
        if (rect.bottom > this->rect.right_bottom.y) {
          f32 y_remaining = this->rect.right_bottom.y - rect.top;
          if (y_remaining < 0) {
            break;
          }
          position.bottom = position.top + y_remaining;
          rect.bottom = this->rect.right_bottom.y;
        }
        // reset x
        rect.left = this->rect.left_top.x;
        rect.right = rect.left + this->size.width;
        position.right = this->position.x + this->size.width;
        while (true) {
          if (rect.right > this->rect.right_bottom.x) {
            f32 x_remaining = this->rect.right_bottom.x - rect.left;
            if (x_remaining < 0) {
              break;
            }
            position.right = position.left + x_remaining;
            rect.right = this->rect.right_bottom.x;
          }
          ID2D1RenderTarget_DrawBitmap(
            this->window->__d2d_render_target, (ID2D1Bitmap*)this->image->__bitmap,
            &rect, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &position
          );
          rect.left += this->size.width;
          rect.right += this->size.width;
        }
        rect.top += this->size.height;
        rect.bottom += this->size.height;
      }
    }
  }
}

#endif