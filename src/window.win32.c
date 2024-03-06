#include "internal/platform.h"

#if PLATFORM_WINDOWS

// window
#include "internal/window.win32.h"
// memory_alloc
#include "internal/memory.h"
// error
#include "sdk/error.h"
// app_main
#include "internal/application.h"
// console_log_cstr
#include "sdk/console.h"
// DXGI_SWAP_CHAIN_DESC
#define COBJMACROS 1
#include <d3d11.h>
#undef COBJMACROS

void window_create_renderer(window_t* window, LPCREATESTRUCT lpc) {
  // create renderer
  DXGI_SWAP_CHAIN_DESC scd = { };
  scd.BufferCount = 1;                                // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
  scd.BufferDesc.Width = lpc->cx;                     // set the back buffer width
  scd.BufferDesc.Height = lpc->cy;                    // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
  scd.OutputWindow = window->handle;                  // the window to be used
  scd.SampleDesc.Count = 4;                           // how many multisamples
  scd.Windowed = TRUE;                                // windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
  D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0,
    0,
    0,
    0,
    D3D11_SDK_VERSION,
    &scd,
    &window->swapchain,
    &window->device,
    0,
    &window->device_context
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
  ID3D11DeviceContext_RSSetViewports(window->device_context, 1, &viewport);
  // GetBackBuffer
  // get the address of the back buffer
  ID3D11Texture2D* backbuffer;
  IDXGISwapChain_GetBuffer(window->swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
  /**
   * use the back buffer address to create the render target
   * set the render target as the back buffer
   */
  ID3D11Device_CreateRenderTargetView(window->device, (ID3D11Resource*)&backbuffer, null, &window->backbuffer);
  ID3D11Texture2D_Release(backbuffer);
  // CreateRasterizerState
  D3D11_RASTERIZER_DESC rasterizer_desc;
  rasterizer_desc.FillMode = D3D11_FILL_SOLID;
  rasterizer_desc.CullMode = D3D11_CULL_NONE;
  ID3D11Device_CreateRasterizerState(window->device, &rasterizer_desc, &window->rasterizer_state);
}

LRESULT window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* window = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
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

    } break;
    case WM_NCCREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      window = (window_t*)pcs->lpCreateParams;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)window);
      window_create_renderer(window, pcs);
    } break;
    case WM_CLOSE: // onClose
      DestroyWindow(handle);
      window->handle = 0;
      break;
    case WM_DESTROY: // onDestroy
      KillTimer(handle, 0);
      window->handle = 0;
      memory_free(window);
      PostQuitMessage(0);
      break;
  }
  return DefWindowProc(handle, message, wParam, lParam);
}

void window_inicialize(window_opt* options) {
  window_t* this = memory_alloc(sizeof(window_t));
  // RegisterWindowClass
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursor(0, IDC_ARROW),
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
  this->handle = CreateWindowExA(
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