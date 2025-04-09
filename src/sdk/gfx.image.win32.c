#include <sdk/gfx.image.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.h>
#include <sdk/window.win32.h>

export void gfx_image_repeat(const gfx_image_t* this, f32 rect[4], f32 src_rect[4]) {
  D2D1_RECT_F position = {
    src_rect[0], src_rect[1],
    src_rect[0] + this->width, src_rect[1] + this->height,
  };
  D2D1_RECT_F local_rect = {
    .top = rect[1],
    .bottom = rect[1] + this->width,
  };
  f32 rect_right_start = rect[0] + this->width;
  f32 position_right_start = position.right;
  while (true) {
    if (local_rect.bottom > rect[3]) {
      f32 y_remaining = rect[3] - local_rect.top;
      if (y_remaining < 0)
        break;
      position.bottom = position.top + y_remaining;
      local_rect.bottom = rect[3];
    }
    if (local_rect.bottom >= 0 && local_rect.top <= global_window_height) {
      // reset x
      local_rect.left = rect[0];
      local_rect.right = rect_right_start;
      position.right = position_right_start;
      while (true) {
        if (local_rect.right > rect[2]) {
          f32 x_remaining = rect[2] - local_rect.left;
          if (x_remaining < 0) {
            break;
          }
          position.right = position.left + x_remaining;
          local_rect.right = rect[2];
        }
        if (local_rect.right >= 0 && local_rect.left <= global_window_width) {
          global_d2d_render_target->lpVtbl->DrawBitmap(
            global_d2d_render_target, (ID2D1Bitmap*)this->src, &local_rect, 1.f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            (D2D1_RECT_F*)&src_rect
          );
        }
        local_rect.left += this->width;
        local_rect.right += this->width;
      }
    }
    local_rect.top += this->height;
    local_rect.bottom += this->height;
  }
}
export void gfx_image_draw(const gfx_image_t* this, f32 rect[4], f32 src_rect[4], f32 opacity) {
  global_d2d_render_target->lpVtbl->DrawBitmap(
    global_d2d_render_target, (ID2D1Bitmap*)this->src, (D2D1_RECT_F*)rect,
    opacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    (D2D1_RECT_F*)src_rect
  );
}
export void gfx_image_src(gfx_image_t* this, const wchar_t* path) {
  assert(global_window);
  assert(path);
  HRESULT result;
  IWICImagingFactory* wic_factory;
  IWICBitmapDecoder* decoder;
  IWICBitmapFrameDecode* frame_decode;
  IWICFormatConverter* converter;
  result = CoCreateInstance(
    &CLSID_WICImagingFactory, null, CLSCTX_INPROC_SERVER,
    &IID_IWICImagingFactory, (void**)&wic_factory
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
  result = global_d2d_render_target->lpVtbl->CreateBitmapFromWicBitmap(
    global_d2d_render_target, (IWICBitmapSource*)converter, null,
    (ID2D1Bitmap**)&this->src
  );
  if (FAILED(result)) {
    error("CreateBitmapFromWicBitmap", result);
  }
  console_log("Bitmap %x", this->src);
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
}
export void gfx_image_free(gfx_image_t* this) {
  ID2D1Bitmap_Release((ID2D1Bitmap*)this->src);
}

#endif