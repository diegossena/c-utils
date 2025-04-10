#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>

#include <sdk/window.win32.h>

titlescreen_t titlescreen;

export void titlescreen_load() {
  // titlescreen.white = gfx_color_new(1, 1, 1, 1);
  // // title
  // titlescreen.title_style = gfx_textstyle_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // // press_space
  // titlescreen.press_space_style = gfx_textstyle_new(font_family_megaman, 26, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // loaded
  titlescreen.loaded = true;
  global_window_repaint = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      // transition_load(titlescreen_unload, hero_home_2f_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  LRESULT result;
  ID3D11Buffer* vertex_buffer;
  IWICBitmapDecoder* decoder = 0;
  ID3D11ShaderResourceView* texture_srv = 0;
  IWICBitmapFrameDecode* frame = 0;
  IWICFormatConverter* converter = 0;
  ID3D11Texture2D* texture = 0;
  // assets
  result = global_wic_factory->lpVtbl->CreateDecoderFromFilename(
    global_wic_factory, L"assets/pallet_town_interiors.png", 0, GENERIC_READ,
    WICDecodeMetadataCacheOnLoad,
    &decoder
  );
  if (FAILED(result)) {
    error("IWICImagingFactory_CreateDecoderFromFilename", result);
  }
  result = decoder->lpVtbl->GetFrame(decoder, 0, &frame);
  if (FAILED(result)) {
    error("IWICBitmapDecoder_GetFrame", result);
  }
  result = global_wic_factory->lpVtbl->CreateFormatConverter(global_wic_factory, &converter);
  if (FAILED(result)) {
    error("IWICImagingFactory_CreateFormatConverter", result);
  }
  // IWICFormatConverter_Inicialize
  result = converter->lpVtbl->Initialize(
    converter,
    (IWICBitmapSource*)frame,
    &GUID_WICPixelFormat32bppRGBA,
    WICBitmapDitherTypeNone,
    0,
    0.,
    WICBitmapPaletteTypeCustom
  );
  if (FAILED(result)) {
    error("IWICFormatConverter_Initialize", result);
  }
  // IWICBitmapFrameDecode_GetSize
  u32 width = 0, height = 0;
  frame->lpVtbl->GetSize(frame, &width, &height);
  u32 stride = width * 4;
  u32 image_size = stride * height;
  u8* image_data = memory_alloc(image_size);
  // IWICFormatConverter_CopyPixels
  result = converter->lpVtbl->CopyPixels(
    converter,
    0, // full rect
    stride,
    image_size,
    image_data
  );
  if (FAILED(result)) {
    error("IWICFormatConverter_CopyPixels", result);
  }
  D3D11_TEXTURE2D_DESC tex_desc = {
    .Width = width,
    .Height = height,
    .MipLevels = 1,
    .ArraySize = 1,
    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .SampleDesc = {.Count = 1, .Quality = 0 },
    .Usage = D3D11_USAGE_DEFAULT,
    .BindFlags = D3D11_BIND_SHADER_RESOURCE,
    .CPUAccessFlags = 0,
    .MiscFlags = 0,
  };
  D3D11_SUBRESOURCE_DATA subresource_data = {
    .pSysMem = image_data,
    .SysMemPitch = stride,
  };
  result = global_d3d_device->lpVtbl->CreateTexture2D(global_d3d_device, &tex_desc, &subresource_data, &texture);
  if (FAILED(result)) {
    error("CreateTexture2D", result);
  }
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
    .Format = tex_desc.Format,
    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
    .Texture2D = {.MipLevels = 1 }
  };
  result = global_d3d_device->lpVtbl->CreateShaderResourceView(
    global_d3d_device, (ID3D11Resource*)texture, &srv_desc, &texture_srv
  );
  if (FAILED(result)) {
    error("CreateShaderResourceView", result);
  }
  global_d3d_device_context->lpVtbl->PSSetShaderResources(global_d3d_device_context, 0, 1, &texture_srv);
  // CreateVertexBuffer
  f32 vertices [][4] = {
    {
      0.0f, 0.5f,
      0.f, 1.f
    },
    {
      0.45f, -0.5,
      .5f, .0f
    },
    {
      -0.45f, -0.5f,
      1.f, 1.f
    }
  };
  const u32 vertex_stride = sizeof(vertices[0]);
  const u64 vertex_count = sizeof(vertices) / vertex_stride;
  const u32 vertex_offset = 0;
  D3D11_BUFFER_DESC vb_desc = {
    .ByteWidth = sizeof(vertices),
    .Usage = D3D11_USAGE_DEFAULT,
    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
  };
  D3D11_SUBRESOURCE_DATA vbData = { vertices };
  result = global_d3d_device->lpVtbl->CreateBuffer(
    global_d3d_device, &vb_desc, &vbData, &vertex_buffer
  );
  if (FAILED(result)) {
    error("VertexShader_CreateBuffer", result);
  }
  global_d3d_device_context->lpVtbl->IASetVertexBuffers(global_d3d_device_context, 0, 1, &vertex_buffer, &vertex_stride, &vertex_offset);
  //
  global_d3d_device_context->lpVtbl->Draw(global_d3d_device_context, vertex_count, 0);
  global_d3d_swapchain->lpVtbl->Present(global_d3d_swapchain, 0, 0);
  // cleanup
  memory_free(image_data);
  texture->lpVtbl->Release(texture);
  converter->lpVtbl->Release(converter);
  frame->lpVtbl->Release(frame);
  decoder->lpVtbl->Release(decoder);
  texture_srv->lpVtbl->Release(texture_srv);
  vertex_buffer->lpVtbl->Release(vertex_buffer);
}