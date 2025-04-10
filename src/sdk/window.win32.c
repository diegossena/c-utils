#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>

HWND global_window;

ID3D11Device* global_d3d_device = 0;
IDXGISwapChain* global_d3d_swapchain = 0;
ID3D11DeviceContext* global_d3d_device_context = 0;
ID3D11RasterizerState* global_d3d_rasterizer = 0;
ID3D11RenderTargetView* global_d3d_render_target_view = 0;

ID3D11InputLayout* global_input_layout;
ID3D11VertexShader* global_vertex_shader;
ID3D11PixelShader* global_pixel_shader;
ID3D11SamplerState* global_sampler_state;

ID3D11ShaderResourceView* global_atlas;

export void _window_resize() {
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
    .Width = (f32)global_window_width,
    .Height = (f32)global_window_height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  global_d3d_device_context->lpVtbl->RSSetViewports(
    global_d3d_device_context, 1, &viewport
  );
}
extern void window_run() {
  f64 time = time_now_f64();
  const f64 frame_rate = 1. / 60.;
  while (global_window_thread) {
    if (global_window_resize) {
      global_d3d_render_target_view->lpVtbl->Release(global_d3d_render_target_view);
      HRESULT result = global_d3d_swapchain->lpVtbl->ResizeBuffers(
        global_d3d_swapchain, 1, global_window_width, global_window_height,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0
      );
      if (FAILED(result)) {
        error("IDXGISwapChain_ResizeBuffers", result);
      }
      _window_resize();
    }
    f64 now = time_now_f64();
    global_gfx_deltatime = now - time;
    if (global_gfx_deltatime > frame_rate) {
      time = now;
      if (global_window_repaint) {
        global_window_repaint = false;
        const f32 color[4] = { 0, 0, 0, 1 };
        window_draw(color);
      }
    }
  }
  // dx11_cleanup
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
export void window_close() { DestroyWindow(global_window); }
export void __window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (global_window_focus && global_window_keyboard_count) {
    window_onkeypress();
  }
}
export void window_startup() {
  assert(global_window_thread == 0);
  sync_t* onload_sync = sync_new();
  global_window_thread = thread_new(_window_thread, onload_sync);
  sync_wait(onload_sync);
  i32 result;
  // global_d3d_device | global_d3d_swapchain | global_d3d_device_context
  DXGI_SWAP_CHAIN_DESC scd = {
    .BufferCount = 1,                                // one back buffer
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
    .BufferDesc.Width = global_window_width,         // set the back buffer width
    .BufferDesc.Height = global_window_height,       // set the back buffer height
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
  _window_resize();
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
  const u8 vs_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x90, 0x39, 0xfe, 0xf5, 0x40, 0xa4, 0xbc, 0xfd, 0x4b, 0xa3, 0x89, 0xde, 0xd4, 0xb6, 0xb7, 0x4f, 0x01, 0x00, 0x00, 0x00, 0x88, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0xac, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x58, 0x01, 0x00, 0x00, 0xec, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xfe, 0xff, 0x00, 0x01, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x4c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x0c, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0x8c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0x23, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5f, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08, 0xc2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
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
  const u8 ps_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x04, 0xc8, 0x5f, 0x90, 0x45, 0x7e, 0x97, 0xa5, 0x96, 0xa7, 0x31, 0x58, 0x18, 0x9c, 0xed, 0x29, 0x01, 0x00, 0x00, 0x00, 0xd4, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x5c, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x38, 0x02, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x73, 0x74, 0x61, 0x74, 0x65, 0x00, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x5f, 0x32, 0x64, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0xa0, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x8b, 0xc2, 0x00, 0x00, 0x80, 0x43, 0x55, 0x15, 0x00, 0x72, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x7e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x72, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x82, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
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
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,               0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
    .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
    .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
    .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
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

  // IWICImagingFactory

}
extern void window_atlas_load(const wchar_t* path) {
  assert(global_atlas == 0);
  i32 result;
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
  CoUninitialize();
  // decoder
  IWICBitmapDecoder* decoder;
  result = wic_factory->lpVtbl->CreateDecoderFromFilename(
    wic_factory, path, 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder
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
  u32 width = 0, height = 0;
  result = frame->lpVtbl->GetSize(frame, &width, &height);
  if (FAILED(result)) {
    error("GetSize", result);
  }
  frame->lpVtbl->Release(frame);
  // image_load
  const u32 image_stride = width * 4;
  const u32 image_size = image_stride * height;
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
  // CreateTexture2D
  ID3D11Texture2D* texture;
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
  result = global_d3d_device->lpVtbl->CreateTexture2D(
    global_d3d_device, &texture_desc, &subresource_data, &texture
  );
  if (FAILED(result)) {
    error("CreateTexture2D", result);
  }
  memory_free(image_data);
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
      global_window_repaint = true;
      ValidateRect(window_id, 0);
      break;
    case WM_KEYDOWN:
      if (global_window_focus) {
        window_onkeydown(wParam);
        if (wParam != 91 && !window_key_pressed(wParam)) {
          u8 byte_index = wParam / 8;
          u8 bit_index = wParam % 8;
          global_window_keyboard_state[byte_index] |= (1 << bit_index);
          ++global_window_keyboard_count;
        }
      }
      return 0;
    case WM_KEYUP:
      if (global_window_focus) {
        window_onkeyup();
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        global_window_keyboard_state[byte_index] &= ~(1 << bit_index);
        --global_window_keyboard_count;
      }
      return 0;
    case WM_SIZE:
      if (global_d3d_device) {
        global_window_width = LOWORD(lParam);
        global_window_height = LOWORD(lParam);
        global_window_resize = true;
      }
      break;
    case WM_SETFOCUS:
      global_window_focus = true;
      return 0;
    case WM_KILLFOCUS:
      global_window_focus = false;
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
export void window_draw(const f32 color[4]) {
  global_d3d_device_context->lpVtbl->ClearRenderTargetView(
    global_d3d_device_context, global_d3d_render_target_view, color
  );
  window_onrender();
  // global_d3d_swapchain->lpVtbl->Present(global_d3d_swapchain, 0, 0);
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
  u32 window_style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindowExA
  RECT rect = { 0, 0, global_window_width, global_window_height };
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
        thread_free(global_window_thread);
        global_window_thread = 0;
        return;
      }
    }
  } while (true);
}

#endif