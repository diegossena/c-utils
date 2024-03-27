#ifdef SDK_WINDOW_GFX_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

typedef struct gfx_image_src_t {
  ID2D1Bitmap* __bitmap;
  const u16 width, height;
} gfx_image_src_t;

void gfx_image_src_new(gfx_image_src_t* this, const wchar_t* path, const window_t* window) {
  assert(path);
  assert(window);
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
  __leaks_memory_increment();
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
}
void gfx_image_src_free(gfx_image_src_t* this) {
  ID2D1Bitmap_Release(this->__bitmap);
  __leaks_memory_decrement();
}
void gfx_image_draw(const gfx_image_t* this) {
  const window_t* window = this->window;
  ID2D1RenderTarget* render_target = this->window->__d2d_render_target;
  ID2D1Bitmap* bitmap = this->src->__bitmap;
  D2D1_RECT_F position = {
    this->src_position.x, this->src_position.y,
    this->src_position.x + this->src_width, this->src_position.y + this->src_height,
  };
  switch (this->extend_mode) {
    case BITMAP_EXTEND_COVER:
      ID2D1RenderTarget_DrawBitmap(
        render_target, bitmap, (D2D1_RECT_F*)&this->rect, 1.f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, (D2D1_RECT_F*)&position
      );
      break;
    case BITMAP_EXTEND_NO_REPEAT: {
      D2D1_RECT_F rect = {
        .left = this->rect.left_top.x,
        .top = this->rect.left_top.y,
        .right = rect.left + this->src_width,
        .bottom = rect.top + this->src_height,
      };
      ID2D1RenderTarget_DrawBitmap(
        render_target, bitmap, &rect, 1.f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, (D2D1_RECT_F*)&position
      );
    } break;
    default: {
      // repeat
      D2D1_RECT_F rect = {
        .top = this->rect.left_top.y,
        .bottom = this->rect.left_top.y + this->src_height,
      };
      f32 rect_right_start = this->rect.left_top.x + this->src_width;
      f32 position_right_start = position.right;
      while (true) {
        if (rect.bottom > this->rect.right_bottom.y) {
          f32 y_remaining = this->rect.right_bottom.y - rect.top;
          if (y_remaining < 0) {
            break;
          }
          position.bottom = position.top + y_remaining;
          rect.bottom = this->rect.right_bottom.y;
        }
        if (rect.bottom >= 0 && rect.top <= window->height) {
          // reset x
          rect.left = this->rect.left_top.x;
          rect.right = rect_right_start;
          position.right = position_right_start;
          while (true) {
            if (rect.right > this->rect.right_bottom.x) {
              f32 x_remaining = this->rect.right_bottom.x - rect.left;
              if (x_remaining < 0) {
                break;
              }
              position.right = position.left + x_remaining;
              rect.right = this->rect.right_bottom.x;
            }
            if (rect.right >= 0 && rect.left <= window->width) {
              ID2D1RenderTarget_DrawBitmap(
                render_target, bitmap, &rect, 1.f,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, (D2D1_RECT_F*)&position
              );
            }
            rect.left += this->src_width;
            rect.right += this->src_width;
          }
        }
        rect.top += this->src_height;
        rect.bottom += this->src_height;
      }
    }
  }
}

#endif
#endif