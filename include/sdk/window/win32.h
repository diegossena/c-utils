#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>

typedef struct window_t {
  void* context;
  u16 width, height;
  vector2d_t cursor;
  bool has_focus;
  // timer
  f64 __updated_at;
  f32 elapsed_time;
  // event_listener_t
  queue_t onupdate;
  queue_t ondraw;
  queue_t onmousemove;
  queue_t onmousedown;
  queue_t onmouseup;
  queue_t ondblclick;
#ifdef SDK_KEYBOARD_H
  queue_t onkeypress;
#endif
  queue_t onkeydown;
  queue_t onkeyup;
  queue_t onresize;
  queue_t onclose;
  // window
  HWND __hwnd;
  bool __mouse_tracking;
#ifdef SDK_WINDOW_GFX_TEXT_H
  queue_t __fonts; // __font_queue_t
  IDWriteFontCollection* __collection;
#endif
  // d3d_renderer
  IDXGISwapChain* __d3d_swapchain;
  ID3D11Device* __d3d_device;
  ID3D11DeviceContext* __d3d_device_context;
  ID3D11RenderTargetView* __d3d_backbuffer;
  ID3D11RasterizerState* __d3d_rasterizer;
  ID3D11VertexShader* __d3d_vertex_shader;
  ID3D11InputLayout* __d3d_input_layout;
  ID3D11PixelShader* __d3d_pixel_shader;
  // d2d_renderer
  ID2D1Factory* __d2d_factory;
  IDXGISurface* __d2d_surface;
  ID2D1RenderTarget* __d2d_render_target;
  IDWriteFactory* __d2d_write_factory;
} window_t;