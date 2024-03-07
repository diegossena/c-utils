#pragma once

#include "sdk/window.h"
#include "internal/task.h"
#include <windows.h>
#define COBJMACROS 1
#include <d3d11.h>
#undef COBJMACROS

typedef struct window_t {
  HWND handle;
  void* context;
  // renderer
  IDXGISwapChain* swapchain;
  ID3D11Device* device;
  ID3D11DeviceContext* device_context;
  ID3D11RenderTargetView* backbuffer;
  ID3D11RasterizerState* rasterizer_state;
  ID3D11VertexShader* vertex_shader;
  ID3D11InputLayout* input_layout;
  ID3D11PixelShader* pixel_shader;
} window_t;