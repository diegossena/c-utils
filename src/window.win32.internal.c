#include "internal/platform.h"

#if PLATFORM_WINDOWS

// windows.h, d3d11.h, sdk/window.h
#include "internal/window.win32.h"
#include "internal/memory.h"
#include "internal/application.h"
#include "internal/string.h"
#include "sdk/error.h"
#include "sdk/console.h"
#include "sdk/process.h"

#define wpath_dirname(this, length) { \
  wchar_t* __ptr = this + length - 1; \
  while (--length && *__ptr != L'\\' && *__ptr != L'/') { \
    --__ptr; \
  } \
  *__ptr = L'\0'; \
}
#define wpath_join(this, length, src, src_length) { \
  wchar_t* __ptr = this + length - 1; \
  if (*__ptr != L'\\' && *__ptr != L'/') { \
    *++__ptr =  L'\\'; \
    ++length; \
  } \
  __ptr = this + length; \
  memory_copy(__ptr, src, sizeof(wchar_t) * src_length + 2); \
  __ptr += length; \
  *__ptr = L'\0'; \
  length += length; \
}
#define wpath_join_cwstr(this, length, cstrw) { \
  u64 __cstrw_length = sizeof(cstrw) / 2 - 2; \
  wpath_join(this, length, cstrw, __cstrw_length) \
}

void shader_load_2d_paint(window_t* window) {
  if (window->input_layout) {
    ID3D11InputLayout_Release(window->input_layout);
    ID3D11VertexShader_Release(window->vertex_shader);
    ID3D11PixelShader_Release(window->pixel_shader);
  }
  ID3DBlob* blob;
  HRESULT result;
  wchar_t path[MAX_PATH] = {};
  u64 path_length = GetModuleFileNameW(NULL, path, MAX_PATH);
  wpath_dirname(path, path_length);
  // layout
  D3D11_INPUT_ELEMENT_DESC ied [] = {
    {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
  // vertex_shader
  wpath_join_cwstr(path, path_length, L"vs_2d_paint.cso");
  result = D3DReadFileToBlob(path, &blob);
  if (result != 0) {
    error("D3DReadFileToBlob::vs_2d_paint.cso", result);
    return;
  }
  result = ID3D11Device_CreateVertexShader(
    window->device, ID3D10Blob_GetBufferPointer(blob),
    ID3D10Blob_GetBufferSize(blob), 0, &window->vertex_shader
  );
  if (result != S_OK) {
    error("CreateVertexShader::2d_paint", result);
    goto vertex_shader_free;
  }
  // input_layout
  result = ID3D11Device_CreateInputLayout(
    window->device, ied, 2, ID3D10Blob_GetBufferPointer(blob),
    ID3D10Blob_GetBufferSize(blob), &window->input_layout
  );
  if (result != S_OK) {
    error("CreateInputLayout", result);
    goto input_layout_free;
  }
  wpath_dirname(path, path_length);
  wpath_join_cwstr(path, path_length, L"ps_2d_paint.cso");
  ID3D10Blob_Release(blob);
  result = D3DReadFileToBlob(path, &blob);
  if (result != 0) {
    error("D3DReadFileToBlob::ps_2d_paint.cso", result);
    goto pixel_shader_free;
  }
  result = ID3D11Device_CreatePixelShader(
    window->device, ID3D10Blob_GetBufferPointer(blob),
    ID3D10Blob_GetBufferSize(blob), 0, &window->pixel_shader
  );
  if (result != S_OK) {
    error("ID3D11Device_CreatePixelShader::2d_paint", result);
    goto pixel_shader_free;
  }
  return;
pixel_shader_free:
  ID3D11PixelShader_Release(window->pixel_shader);
  window->pixel_shader = 0;
input_layout_free:
  ID3D11InputLayout_Release(window->input_layout);
  window->input_layout = 0;
vertex_shader_free:
  ID3D11VertexShader_Release(window->vertex_shader);
  window->vertex_shader = 0;
  ID3D10Blob_Release(blob);
}
void renderer_set_viewport(window_t* this, i32 width, i32 height) {
  if (this->backbuffer) {
    ID2D1RenderTarget_Release(this->d2_render_target);
    IDXGISurface_Release(this->d2_surface);
    ID3D11RenderTargetView_Release(this->backbuffer);
  }
  D3D11_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = (f32)width,
    .Height = (f32)height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  ID3D11DeviceContext_RSSetViewports(this->device_context, 1, &viewport);
  IDXGISwapChain_ResizeBuffers(
    this->swapchain, 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0
  );
  // get backbuffer
  ID3D11Texture2D* backbuffer;
  IDXGISwapChain_GetBuffer(this->swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
  /**
   * use the back buffer address to create the render target
   * set the render target as the back buffer
   */
  ID3D11Device_CreateRenderTargetView(
    this->device, (ID3D11Resource*)backbuffer, null, &this->backbuffer
  );
  ID3D11DeviceContext_OMSetRenderTargets(
    this->device_context, 1, &this->backbuffer, 0
  );
  // set d2_render_target
  ID3D11Texture2D_QueryInterface(
    backbuffer, &IID_IDXGISurface, (void**)&this->d2_surface
  );
  D2D1_RENDER_TARGET_PROPERTIES renter_target_props = {
    .type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
    .pixelFormat = {
      .format = DXGI_FORMAT_UNKNOWN,
      .alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED
    }
  };
  HRESULT result = ID2D1Factory_CreateDxgiSurfaceRenderTarget(
    this->d2_factory, this->d2_surface, &renter_target_props,
    &this->d2_render_target
  );
  if (FAILED(result)) {
    error("CreateDxgiSurfaceRenderTarget", result);
  }
  // free resources
  ID3D11Texture2D_Release(backbuffer);
}
void renderer_inicialize(window_t* this, LPCREATESTRUCT lpc) {
  // d2 inicialize
  HRESULT result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, null,
    (void**)&this->d2_factory
  );
  if (FAILED(result)) {
    error("D2D1CreateFactory", result);
  }
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&this->d2_write_factory
  );
  if (FAILED(result)) {
    error("DWriteCreateFactory", result);
  }
  // create renderer
  DXGI_SWAP_CHAIN_DESC scd = { };
  scd.BufferCount = 1;                                // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
  scd.BufferDesc.Width = lpc->cx;                     // set the back buffer width
  scd.BufferDesc.Height = lpc->cy;                    // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
  scd.OutputWindow = this->handle;                    // the window to be used
  scd.SampleDesc.Count = 4;                           // how many multisamples
  scd.Windowed = TRUE;                                // windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
  result = D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0,
    // Required for Direct2D interoperability with Direct3D resources.
    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    0,
    0,
    D3D11_SDK_VERSION,
    &scd,
    &this->swapchain,
    &this->device,
    0,
    &this->device_context
  );
  if (FAILED(result)) {
    console_log("D3D11CreateDeviceAndSwapChain %x", result);
    exit(1);
  }
  renderer_set_viewport(this, lpc->cx, lpc->cy);
  // CreateRasterizerState
  D3D11_RASTERIZER_DESC rasterizer_desc = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE
  };
  ID3D11Device_CreateRasterizerState(this->device, &rasterizer_desc, &this->rasterizer_state);
  // CreateShaders
  shader_load_2d_paint(this);
}

void window_free(window_t* this) {
  // D2D
  ID2D1RenderTarget_Release(this->d2_render_target);
  IDXGISurface_Release(this->d2_surface);
  ID2D1Factory_Release(this->d2_factory);
  // D3D
  ID3D11PixelShader_Release(this->pixel_shader);
  ID3D11InputLayout_Release(this->input_layout);
  ID3D11VertexShader_Release(this->vertex_shader);
  ID3D11RasterizerState_Release(this->rasterizer_state);
  ID3D11RenderTargetView_Release(this->backbuffer);
  ID3D11DeviceContext_Release(this->device_context);
  ID3D11Device_Release(this->device);
  IDXGISwapChain_Release(this->swapchain);
  // window_t
  this->handle = 0;
  memory_free(this);
}

void window_pooling() {
  MSG msg;
  WINBOOL result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
  if (result) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      --app_global.tasks_count;
      return;
    }
  }
}

LRESULT window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_TIMER:
      break;
    case WM_PAINT: {
      const FLOAT clearColor [] = { 1.0f, 1.0f, 1.0f, 1.0f };
      ID3D11DeviceContext_ClearRenderTargetView(
        this->device_context, this->backbuffer, clearColor
      );
      ID2D1RenderTarget_BeginDraw(this->d2_render_target);
      const WCHAR text [] = L"Hello, World!";
      u64 text_length = sizeof(text) / sizeof(WCHAR) - 1;
      D2D1_RECT_F layoutRect = { 10.0f, 10.0f, 300.0f, 100.0f };
      D2D1_COLOR_F color = { 0.f, 0.f, 0.f, 1.f };

      ID2D1SolidColorBrush* text_brush;
      ID2D1RenderTarget_CreateSolidColorBrush(
        this->d2_render_target, &color, null, &text_brush
      );

      IDWriteTextFormat* text_format;
      IDWriteFactory_CreateTextFormat(
        this->d2_write_factory, L"Arial", null, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US",
        &text_format
      );

      ID2D1RenderTarget_DrawText(
        this->d2_render_target, text, text_length, text_format, &layoutRect,
        (ID2D1Brush*)text_brush, D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
      );

      ID2D1RenderTarget_EndDraw(this->d2_render_target, null, null);
      IDXGISwapChain_Present(this->swapchain, 0, 0);
    } break;
    case WM_SIZE: {
      UINT width = LOWORD(lParam);
      UINT height = HIWORD(lParam);
      renderer_set_viewport(this, width, height);
    } break;
    case WM_NCCREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      this = (window_t*)pcs->lpCreateParams;
      this->handle = handle;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)this);
      renderer_inicialize(this, pcs);
    } break;
    case WM_CLOSE: // onClose
      DestroyWindow(handle);
      break;
    case WM_DESTROY: // onDestroy
      KillTimer(handle, 0);
      PostQuitMessage(0);
      window_free(this);
      break;
  }
  return DefWindowProc(handle, message, wParam, lParam);
}

#endif