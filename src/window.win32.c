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

#include <d3dcompiler.h>

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
  sizeof(L"a");
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
    goto onend;
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
  goto onend;
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
onend:
  return;
}

void window_renderer_inicialize(window_t* this, LPCREATESTRUCT lpc) {
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
  D3D11CreateDeviceAndSwapChain(
    0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &scd,
    &this->swapchain, &this->device, 0, &this->device_context
  );
  // set viewports
  D3D11_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = (f32)lpc->cx,
    .Height = (f32)lpc->cy,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  ID3D11DeviceContext_RSSetViewports(this->device_context, 1, &viewport);
  // GetBackBuffer
  ID3D11Texture2D* backbuffer;
  IDXGISwapChain_GetBuffer(this->swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
  /**
   * use the back buffer address to create the render target
   * set the render target as the back buffer
   */
  ID3D11Device_CreateRenderTargetView(
    this->device, (ID3D11Resource*)backbuffer, null, &this->backbuffer
  );
  ID3D11Texture2D_Release(backbuffer);
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
  ID3D11PixelShader_Release(this->pixel_shader);
  ID3D11InputLayout_Release(this->input_layout);
  ID3D11VertexShader_Release(this->vertex_shader);
  ID3D11RasterizerState_Release(this->rasterizer_state);
  ID3D11RenderTargetView_Release(this->backbuffer);
  ID3D11DeviceContext_Release(this->device_context);
  ID3D11Device_Release(this->device);
  IDXGISwapChain_Release(this->swapchain);
  this->handle = 0;
  memory_free(this);
}

LRESULT window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_TIMER:
      break;
    case WM_ERASEBKGND: {
      HDC hdc = (HDC)wParam;
      RECT rect;
      GetClientRect(handle, &rect);
      HBRUSH white_background = (HBRUSH)(COLOR_WINDOW + 1);
      FillRect(hdc, &rect, white_background);
      return true;
    }
    case WM_PAINT: {
      // float red [] = { 1.0f, 0.0f, 0.0f, 1.0f };
      // float vertex[4][8] = {
      //   {0.f, -.5f, 0.0f, red},
      //   {.5f, .5f, 0.0f, red},
      //   {.0f, .5f, 0.0f, red},
      //   {.5f, -.5f, 0.0f, red},
      // };
    } break;
    case WM_NCCREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      this = (window_t*)pcs->lpCreateParams;
      this->handle = handle;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)this);
      window_renderer_inicialize(this, pcs);
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

void window_inicialize(window_opt* options) {
  window_t* this = memory_alloc0(sizeof(window_t));
  // RegisterWindowClass
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = window_procedure,
    .hInstance = GetModuleHandleA(null),
    .hCursor = LoadCursor(null, IDC_ARROW),
    .lpszClassName = "MainWClass",
    .hbrBackground = null
  };
  if (!RegisterClassExA(&wc)) {
    error("RegisterClassExA", ERR_UNKNOWN);
    goto clear;
  }
  u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
  if (!(options->flags & WINDOW_HIDDEN)) {
    window_style |= WS_VISIBLE;
  }
  u32 window_ex_style = WS_EX_APPWINDOW;
  CreateWindowExA(
    window_ex_style, wc.lpszClassName, options->name, window_style,
    options->x, options->y, options->width, options->height,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->handle) {
    error("CreateWindowExA", ERR_UNKNOWN);
    goto clear;
  }
  SetTimer(this->handle, 0, 0, 0);
  ++app_global.tasks_count;
  return;
clear:
  memory_free(this);
}

#endif