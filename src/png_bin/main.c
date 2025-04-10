#include <sdk/types.h>
#include <sdk/error.h>
#include <stdio.h>
#include <windows.h>
#include <wincodec.h>

#include <sdk/unity.h>

i32 main(i32 argc, char** argv) {
  i32 result;
  const wchar_t* in_path = L"assets/atlas.png";
  const char* out_path = "assets/atlas.bin";
  // CoInitialize
  result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
  if (FAILED(result)) {
    error("CoInitializeEx", result);
  }
  // wic_factory
  IWICImagingFactory* wic_factory;
  result = CoCreateInstance(
    &CLSID_WICImagingFactory,
    NULL,
    CLSCTX_INPROC_SERVER,
    &IID_IWICImagingFactory,
    (void**)&wic_factory
  );
  if (FAILED(result)) {
    error("CoCreateInstance_IWICImagingFactory", result);
  }
  // decoder
  IWICBitmapDecoder* decoder;
  result = wic_factory->lpVtbl->CreateDecoderFromFilename(
    wic_factory, in_path, 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder
  );
  if (FAILED(result)) {
    error("CreateDecoderFromFilename", result);
  }
  // frame
  IWICBitmapFrameDecode* frame;
  result = decoder->lpVtbl->GetFrame(decoder, 0, &frame);
  if (FAILED(result)) {
    error("IWICBitmapDecoder_GetFrame", result);
  }
  decoder->lpVtbl->Release(decoder);
  // CreateFormatConverter
  IWICFormatConverter* converter;
  result = wic_factory->lpVtbl->CreateFormatConverter(wic_factory, &converter);
  if (FAILED(result)) {
    error("IWICImagingFactory_CreateFormatConverter", result);
  }
  wic_factory->lpVtbl->Release(wic_factory);
  // IWICFormatConverter_Inicialize
  result = converter->lpVtbl->Initialize(
    converter, (IWICBitmapSource*)frame, &GUID_WICPixelFormat32bppRGBA,
    WICBitmapDitherTypeNone, 0, 0., WICBitmapPaletteTypeCustom
  );
  if (FAILED(result)) {
    error("IWICFormatConverter_Initialize", result);
  }
  // IWICBitmapFrameDecode_GetSize
  u32 image_width, image_height;
  result = frame->lpVtbl->GetSize(frame, &image_width, &image_height);
  if (FAILED(result)) {
    error("GetSize", result);
  }
  frame->lpVtbl->Release(frame);
  // image_load
  const u32 image_stride = image_width * 4;
  const u32 image_size = image_stride * image_height;
  u8* image_data = memory_alloc(image_size);
  // IWICFormatConverter_CopyPixels
  result = converter->lpVtbl->CopyPixels(
    converter,
    0, // full rect
    image_stride,
    image_size,
    image_data
  );
  if (FAILED(result)) {
    error("IWICFormatConverter_CopyPixels", result);
  }
  converter->lpVtbl->Release(converter);
  // cache_file
  FILE* file = fopen(out_path, "w");
  if (file == 0) {
    error("fopen", (error_t)file);
  }
  fwrite(&image_width, 1, sizeof(image_width), file);
  fwrite(&image_height, 1, sizeof(image_height), file);
  fwrite(image_data, 1, image_size, file);
  fclose(file);
  // cleanup
  CoUninitialize();
  return 0;
}