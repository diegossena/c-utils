#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <stdio.h>

HWND global_window;

ID3D11Device* global_d3d_device;
IDXGISwapChain* global_d3d_swapchain;
ID3D11DeviceContext* global_d3d_device_context;
ID3D11RasterizerState* global_d3d_rasterizer;
ID3D11RenderTargetView* global_d3d_render_target_view;

ID3D11InputLayout* global_input_layout;
ID3D11VertexShader* global_vertex_shader;
ID3D11PixelShader* global_pixel_shader;

ID3D11SamplerState* global_sampler_state;
ID3D11BlendState* global_blend_state;

ID3D11ShaderResourceView* global_atlas;

ID3D11Buffer* global_vertices_buffer;
ID3D11Buffer* global_indexes_buffer;


export void _window_size_update() {
  ndc_per_px_x = 2.f / (f32)window_width;
  ndc_per_px_y = 2.f / (f32)window_height;
  HRESULT result;
  // global_d3d_render_target_view
  ID3D11Texture2D* back_buffer = 0;
  result = global_d3d_swapchain->lpVtbl->GetBuffer(
    global_d3d_swapchain, 0, &IID_ID3D11Texture2D,
    (void**)&back_buffer
  );
  if (FAILED(result)) {
    error("IDXGISwapChain_GetBuffer", result);
  }
  result = global_d3d_device->lpVtbl->CreateRenderTargetView(
    global_d3d_device, (ID3D11Resource*)back_buffer, 0, &global_d3d_render_target_view
  );
  back_buffer->lpVtbl->Release(back_buffer);
  global_d3d_device_context->lpVtbl->OMSetRenderTargets(
    global_d3d_device_context, 1, &global_d3d_render_target_view, 0
  );
  // viewport
  D3D11_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = (f32)window_width,
    .Height = (f32)window_height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  global_d3d_device_context->lpVtbl->RSSetViewports(
    global_d3d_device_context, 1, &viewport
  );
}
export void window_close() { DestroyWindow(global_window); }
export void __window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (window_focus && keyboard_count) {
    window_onkeypress();
  }
}
export void window_startup() {
  assert(window_thread == 0);
  sync_t* onload_sync = sync_new();
  window_thread = thread_new(_window_thread, onload_sync);
  sync_wait(onload_sync);
  i32 result;
  // global_d3d_device | global_d3d_swapchain | global_d3d_device_context
  DXGI_SWAP_CHAIN_DESC scd = {
    .BufferCount = 1,                                // one back buffer
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
    .BufferDesc.Width = window_width,         // set the back buffer width
    .BufferDesc.Height = window_height,       // set the back buffer height
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,  // how swap chain is to be used
    .OutputWindow = global_window,                   // the window to be used
    .SampleDesc.Count = 4,                           // how many multisamples
    .Windowed = true,                                // windowed/full-screen mode
    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH  // allow full-screen switching
  };
  result = D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0, 0, 0, 0,
    D3D11_SDK_VERSION,
    &scd,
    &global_d3d_swapchain,
    &global_d3d_device,
    0,
    &global_d3d_device_context
  );
  if (FAILED(result)) {
    error("D3D11CreateDeviceAndSwapChain", result);
  }
  global_d3d_device_context->lpVtbl->IASetPrimitiveTopology(global_d3d_device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _window_size_update();
  // global_d3d_rasterizer
  D3D11_RASTERIZER_DESC rasterizer_props = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE
  };
  result = ID3D11Device_CreateRasterizerState(
    global_d3d_device, &rasterizer_props, &global_d3d_rasterizer
  );
  if (FAILED(result)) {
    error("ID3D11Device_CreateRasterizerState", result);
  }
  // global_vertex_shader
  const u8 vs_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x12, 0x63, 0xab, 0x0a, 0x71, 0x74, 0xd2, 0x4c, 0x65, 0x97, 0xf2, 0x8e, 0x53, 0x53, 0xb4, 0x88, 0x01, 0x00, 0x00, 0x00, 0x88, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0xac, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x58, 0x01, 0x00, 0x00, 0xec, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xfe, 0xff, 0x00, 0x01, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x4c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0x8c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0x23, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5f, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08, 0xc2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x36, 0x00, 0x00, 0x05, 0xf2, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  result = global_d3d_device->lpVtbl->CreateVertexShader(
    global_d3d_device,
    vs_cso,
    sizeof(vs_cso),
    0,
    &global_vertex_shader
  );
  if (FAILED(result)) {
    error("CreateVertexShader", result);
  }
  global_d3d_device_context->lpVtbl->VSSetShader(global_d3d_device_context, global_vertex_shader, NULL, 0);
  // global_texture_pixel_shader
  const u8 ps_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x4d, 0x49, 0x7a, 0x88, 0x11, 0x67, 0x10, 0x16, 0x1e, 0xc7, 0xf2, 0x08, 0x81, 0xb5, 0x0e, 0xba, 0x01, 0x00, 0x00, 0x00, 0xec, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x5c, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x50, 0x02, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x73, 0x74, 0x61, 0x74, 0x65, 0x00, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x5f, 0x32, 0x64, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0xb8, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x07, 0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x8b, 0xc2, 0x00, 0x00, 0x80, 0x43, 0x55, 0x15, 0x00, 0xf2, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x7e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x09, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  result = global_d3d_device->lpVtbl->CreatePixelShader(
    global_d3d_device,
    ps_cso,
    sizeof(ps_cso),
    0,
    &global_pixel_shader
  );
  if (FAILED(result)) {
    error("CreatePixelShader", result);
  }
  global_d3d_device_context->lpVtbl->PSSetShader(global_d3d_device_context, global_pixel_shader, NULL, 0);
  // global_texture_input_layout
  D3D11_INPUT_ELEMENT_DESC texture_layout [] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,                     0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  const u8 texture_layout_size = sizeof(texture_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  result = global_d3d_device->lpVtbl->CreateInputLayout(
    global_d3d_device,
    texture_layout,
    texture_layout_size,
    vs_cso,
    sizeof(vs_cso),
    &global_input_layout
  );
  if (FAILED(result)) {
    error("CreateInputLayout", result);
  }
  global_d3d_device_context->lpVtbl->IASetInputLayout(global_d3d_device_context, global_input_layout);
  // global_sampler_state
  const D3D11_SAMPLER_DESC sampler_desc = {
    .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
    .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
    .ComparisonFunc = D3D11_COMPARISON_NEVER,
    .MinLOD = 0,
    .MinLOD = D3D11_FLOAT32_MAX
  };
  result = global_d3d_device->lpVtbl->CreateSamplerState(
    global_d3d_device, &sampler_desc, &global_sampler_state
  );
  if (FAILED(result)) {
    error("CreateSamplerState", result);
  }
  global_d3d_device_context->lpVtbl->PSSetSamplers(global_d3d_device_context, 0, 1, &global_sampler_state);
  // ID3D11Device_CreateBlendState
  D3D11_BLEND_DESC blend_desc = {
    .RenderTarget[0] = {
      .BlendEnable = true,
      .SrcBlend = D3D11_BLEND_SRC_ALPHA,
      .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
      .BlendOp = D3D11_BLEND_OP_ADD,
      .SrcBlendAlpha = D3D11_BLEND_ONE,
      .DestBlendAlpha = D3D11_BLEND_ZERO,
      .BlendOpAlpha = D3D11_BLEND_OP_ADD,
      .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
    }
  };
  global_d3d_device->lpVtbl->CreateBlendState(
    global_d3d_device, &blend_desc, &global_blend_state
  );
  float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
  global_d3d_device_context->lpVtbl->OMSetBlendState(
    global_d3d_device_context, global_blend_state, blend_factor, 0xffffffff
  );
  // global_vertices
  vertices_capacity = 400;
  D3D11_BUFFER_DESC buffer_desc = {
    .Usage = D3D11_USAGE_DYNAMIC,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
  };
  vertices_virtual = memory_alloc(sizeof(vertex_t) * vertices_capacity);
  buffer_desc.ByteWidth = vertices_capacity * sizeof(vertex_t);
  buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  result = global_d3d_device->lpVtbl->CreateBuffer(
    global_d3d_device, &buffer_desc, 0, &global_vertices_buffer
  );
  if (FAILED(result)) {
    error("CreateVertexBuffer", result);
  }
  const u32 vertex_stride = sizeof(vertex_t);
  const u32 vertex_offset = 0;
  global_d3d_device_context->lpVtbl->IASetVertexBuffers(
    global_d3d_device_context, 0, 1, &global_vertices_buffer, &vertex_stride,
    &vertex_offset
  );
  // global_indexes
  indexes_capacity = 600;
  indexes_virtual = memory_alloc(sizeof(u32) * indexes_capacity);
  buffer_desc.ByteWidth = sizeof(u32) * indexes_capacity;
  buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  result = global_d3d_device->lpVtbl->CreateBuffer(
    global_d3d_device, &buffer_desc, 0, &global_indexes_buffer
  );
  if (FAILED(result)) {
    error("CreateIndexBuffer", result);
  }
  global_d3d_device_context->lpVtbl->IASetIndexBuffer(global_d3d_device_context, global_indexes_buffer, DXGI_FORMAT_R32_UINT, 0);
}
extern void window_atlas_load(const char* path, const u64 width, const u64 height) {
  assert(global_atlas == 0);
  i32 result;
  // load_image
  u64 image_size = width * height * 4; // RGBA8
  const u32 image_stride = width * 4;
  FILE* file = fopen(path, "r");
  u8 image_data[image_size];
  fread(image_data, 1, image_size, file);
  fclose(file);
  // CreateTexture2D
  D3D11_TEXTURE2D_DESC texture_desc = {
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
    .SysMemPitch = image_stride,
  };
  ID3D11Texture2D* texture;
  result = global_d3d_device->lpVtbl->CreateTexture2D(
    global_d3d_device, &texture_desc, &subresource_data, &texture
  );
  if (FAILED(result)) {
    error("CreateTexture2D", result);
  }
  // CreateShaderResourceView
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
    .Format = texture_desc.Format,
    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
    .Texture2D.MipLevels = 1
  };
  result = global_d3d_device->lpVtbl->CreateShaderResourceView(
    global_d3d_device, (ID3D11Resource*)texture, &srv_desc, &global_atlas
  );
  if (FAILED(result)) {
    error("CreateShaderResourceView", result);
  }
  // PSSetShaderResources
  global_d3d_device_context->lpVtbl->PSSetShaderResources(global_d3d_device_context, 0, 1, &global_atlas);
}
LRESULT _window_procedure(HWND window_id, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT:
      // global_window_repaint = true;
      ValidateRect(window_id, 0);
      break;
    case WM_KEYDOWN:
      if (window_focus) {
        window_onkeydown(wParam);
        if (wParam != 91 && !window_key_pressed(wParam)) {
          u8 byte_index = wParam / 8;
          u8 bit_index = wParam % 8;
          keyboard_state[byte_index] |= (1 << bit_index);
          ++keyboard_count;
        }
      }
      return 0;
    case WM_KEYUP:
      if (window_focus) {
        window_onkeyup();
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        keyboard_state[byte_index] &= ~(1 << bit_index);
        --keyboard_count;
      }
      return 0;
    case WM_SIZE:
      if (global_d3d_device) {
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        window_resized = true;
      }
      break;
    case WM_SETFOCUS:
      window_focus = true;
      return 0;
    case WM_KILLFOCUS:
      window_focus = false;
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcA(window_id, message, wParam, lParam);
}
export void window_set_title(const char* title) {
  assert(global_window != 0);
  SetWindowTextA(global_window, title);
}
export void _window_thread(sync_t* onload_sync) {
  i32 result;
  const char* title = " ";
  // window_class_register
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = _window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .lpszClassName = title,
  };
  result = RegisterClassExA(&wc);
  if (!result) {
    error("RegisterClassExA", result);
  }
  u32 window_style = WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindowExA
  RECT rect = { 0, 0, window_width, window_height };
  AdjustWindowRect(&rect, window_style, false);
  global_window = CreateWindowExA(
    window_ex_style, wc.lpszClassName, title, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    0 // pointer to window-creation data
  );
  if (!global_window) {
    error("CreateWindowExA", (error_t)global_window);
  }
  sync_signal(onload_sync);
  SetTimer(0, 0, 0, (TIMERPROC)__window_onupdate);
  MSG msg;
  do {
    MsgWaitForMultipleObjectsEx(
      0,
      0,
      INFINITE,
      QS_ALLINPUT,
      MWMO_INPUTAVAILABLE
    );
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        KillTimer(0, 0);
        thread_free(window_thread);
        window_thread = 0;
        return;
      }
    }
  } while (true);
}
extern void window_run() {
  f64 time = time_now_f64();
  const f64 frame_rate = 1. / 60.;
  while (window_thread) {
    if (window_resized) {
      global_d3d_render_target_view->lpVtbl->Release(global_d3d_render_target_view);
      HRESULT result = global_d3d_swapchain->lpVtbl->ResizeBuffers(
        global_d3d_swapchain, 1, window_width, window_height,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0
      );
      if (FAILED(result)) {
        error("IDXGISwapChain_ResizeBuffers", result);
      }
      _window_size_update();
      window_has_update = true;
      window_resized = false;
    }
    f64 now = time_now_f64();
    window_deltatime = now - time;
    if (window_deltatime > frame_rate) {
      time = now;
      if (window_has_update) {
        window_has_update = false;
        // render
        vertices_length = 0;
        indexes_length = 0;
        window_onrender();
        D3D11_MAPPED_SUBRESOURCE subresource;
        // vertices
        global_d3d_device_context->lpVtbl->Map(
          global_d3d_device_context, (ID3D11Resource*)global_vertices_buffer, 0,
          D3D11_MAP_WRITE_DISCARD, 0, &subresource
        );
        memory_copy(subresource.pData, vertices_virtual, sizeof(vertex_t) * vertices_length);
        global_d3d_device_context->lpVtbl->Unmap(
          global_d3d_device_context, (ID3D11Resource*)global_vertices_buffer, 0
        );
        // indices
        global_d3d_device_context->lpVtbl->Map(
          global_d3d_device_context, (ID3D11Resource*)global_indexes_buffer, 0,
          D3D11_MAP_WRITE_DISCARD, 0, &subresource
        );
        memory_copy(subresource.pData, indexes_virtual, sizeof(u32) * indexes_length);
        global_d3d_device_context->lpVtbl->Unmap(
          global_d3d_device_context, (ID3D11Resource*)global_indexes_buffer, 0
        );
        // draw
        global_d3d_device_context->lpVtbl->ClearRenderTargetView(
          global_d3d_device_context, global_d3d_render_target_view, window_background
        );
        global_d3d_device_context->lpVtbl->DrawIndexed(global_d3d_device_context, indexes_length, 0, 0);
        global_d3d_swapchain->lpVtbl->Present(global_d3d_swapchain, 0, 0);
      }
    }
  }
  // dx11_cleanup
  memory_free(indexes_virtual);
  memory_free(vertices_virtual);
  global_indexes_buffer->lpVtbl->Release(global_indexes_buffer);
  global_vertices_buffer->lpVtbl->Release(global_vertices_buffer);
  global_blend_state->lpVtbl->Release(global_blend_state);
  global_sampler_state->lpVtbl->Release(global_sampler_state);
  global_pixel_shader->lpVtbl->Release(global_pixel_shader);
  global_vertex_shader->lpVtbl->Release(global_vertex_shader);
  global_input_layout->lpVtbl->Release(global_input_layout);
  ID3D11RasterizerState_Release(global_d3d_rasterizer);
  ID3D11RenderTargetView_Release(global_d3d_render_target_view);
  ID3D11DeviceContext_Release(global_d3d_device_context);
  ID3D11Device_Release(global_d3d_device);
  IDXGISwapChain_Release(global_d3d_swapchain);
}

#endif