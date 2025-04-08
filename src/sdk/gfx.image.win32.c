#include <sdk/gfx.image.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.h>
#include <sdk/window.win32.h>

export void gfx_image_draw(const gfx_image_t* this) {
  ID2D1Bitmap* bitmap = (ID2D1Bitmap*)this->src;
  D2D1_RECT_F position = {
    this->src_rect[0], this->src_rect[1],
    this->src_rect[0] + this->src_width, this->src_rect[1] + this->src_height,
  };
  switch (this->extend_mode) {
    case BITMAP_EXTEND_COVER:
      global_d2d_render_target->lpVtbl->DrawBitmap(
        global_d2d_render_target, bitmap, (D2D1_RECT_F*)&this->rect, 1.f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        (D2D1_RECT_F*)&this->src_rect
      );
      break;
    case BITMAP_EXTEND_NO_REPEAT: {
      D2D1_RECT_F rect = {
        .left = this->rect[0],
        .top = this->rect[1],
        .right = rect.left + this->src_width,
        .bottom = rect.top + this->src_height,
      };
      ID2D1RenderTarget_DrawBitmap(
        global_d2d_render_target, bitmap, &rect, 1.f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        (D2D1_RECT_F*)&this->src_rect
      );
    } break;
    default: {
      // repeat
      D2D1_RECT_F rect = {
        .top = this->rect[1],
        .bottom = this->rect[1] + this->src_width,
      };
      f32 rect_right_start = this->rect[0] + this->src_width;
      f32 position_right_start = position.right;
      while (true) {
        if (rect.bottom > this->rect[3]) {
          f32 y_remaining = this->rect[3] - rect.top;
          if (y_remaining < 0) {
            break;
          }
          position.bottom = position.top + y_remaining;
          rect.bottom = this->rect[3];
        }
        if (rect.bottom >= 0 && rect.top <= global_window_height) {
          // reset x
          rect.left = this->rect[0];
          rect.right = rect_right_start;
          position.right = position_right_start;
          while (true) {
            if (rect.right > this->rect[2]) {
              f32 x_remaining = this->rect[2] - rect.left;
              if (x_remaining < 0) {
                break;
              }
              position.right = position.left + x_remaining;
              rect.right = this->rect[2];
            }
            if (rect.right >= 0 && rect.left <= global_window_width) {
              ID2D1RenderTarget_DrawBitmap(
                global_d2d_render_target, bitmap, &rect, 1.f,
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
export void gfx_image_src(gfx_image_t* this, const wchar_t* path) {
  assert(global_window);
  assert(path);
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
  frame_decode->lpVtbl->GetSize(frame_decode, (UINT*)&this->src_width, (UINT*)&this->src_height);
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
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
}
export void gfx_image_free(gfx_image_t* this) {
  ID2D1Bitmap* bitmap = (ID2D1Bitmap*)this->src;
  ID2D1Bitmap_Release(bitmap);
}

#endif