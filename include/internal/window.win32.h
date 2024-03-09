#pragma once

#include "sdk/window.h"
#include "internal/task.h"
#include "internal/gfx.win32.h"
#include <windows.h>

typedef struct window_t {
  HWND handle;
  void* context;
  window_update_ev onupdate;
  window_mouse_ev onmousemove;
  window_mouse_ev onmousedown;
  window_mouse_ev onmouseup;
  window_keyboard_ev onkeydown;
  window_keyboard_ev onkeyup;
  window_keyboard_ev ondblclick;
  window_ui_ev onresize;
  window_ev oncreate;
  window_ev onclose;
  // 3d_renderer
  IDXGISwapChain* swapchain;
  ID3D11Device* device;
  ID3D11DeviceContext* device_context;
  ID3D11RenderTargetView* backbuffer;
  ID3D11RasterizerState* rasterizer_state;
  ID3D11VertexShader* vertex_shader;
  ID3D11InputLayout* input_layout;
  ID3D11PixelShader* pixel_shader;
  // 2d_renderer
  ID2D1Factory* d2_factory;
  IDXGISurface* d2_surface;
  ID2D1RenderTarget* d2_render_target;
  IDWriteFactory* d2_write_factory;
} window_t;

LRESULT window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);