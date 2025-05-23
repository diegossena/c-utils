#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <stdio.h>
#include <avrt.h>

HWND window_id;
thread_t* _renderer_thread_id = 0;

ID3D11Device* d3d_device = 0;
IDXGISwapChain* d3d_swapchain;
ID3D11DeviceContext* d3d_device_context;
ID3D11RasterizerState* d3d_rasterizer;
ID3D11RenderTargetView* d3d_render_target_view;

ID3D11InputLayout* d3d_input_layout;
ID3D11VertexShader* d3d_vertex_shader;
ID3D11PixelShader* d3d_pixel_shader;

ID3D11SamplerState* d3d_sampler_state;
ID3D11BlendState* d3d_blend_state;

ID3D11ShaderResourceView* d3d_shader_resource;

ID3D11Buffer* vertices_buffer;
ID3D11Buffer* indexes_buffer;

bool window_resized = true;

export void _indexes_free() {
  indexes_capacity = 0;
  vertices_buffer->lpVtbl->Release(vertices_buffer);
  memory_free(indexes_virtual);
}
export void _vertices_free() {
  vertices_capacity = 0;
  vertices_buffer->lpVtbl->Release(vertices_buffer);
  memory_free(vertices_virtual);
}
export void _window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (window_focus && keyboard_count) {
    window_onkeypress();
  }
}
LRESULT _window_procedure(HWND window_id, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT:
      ValidateRect(window_id, 0);
      break;
    case WM_KEYDOWN:
      if (window_focus) {
        if (wParam != 91 && !window_key_pressed(wParam)) {
          u8 byte_index = wParam / 8;
          u8 bit_index = wParam % 8;
          keyboard_state[byte_index] |= (1 << bit_index);
          ++keyboard_count;
          window_onkeydown(wParam);
        }
      }
      return 0;
    case WM_KEYUP:
      if (window_focus) {
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        keyboard_state[byte_index] &= ~(1 << bit_index);
        --keyboard_count;
        window_onkeyup();
      }
      return 0;
    case WM_MOUSEMOVE:
      mouse_x = GET_X_LPARAM(lParam);
      mouse_y = GET_Y_LPARAM(lParam);
      window_onmousemove();
      return 0;
    case WM_MOUSELEAVE:
      return 0;
    case WM_LBUTTONDOWN:
      window_onmousedown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_LEFT);
      return 0;
    case WM_RBUTTONDOWN:
      window_onmousedown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_MBUTTONDOWN:
      window_onmousedown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_XBUTTONDOWN:
      window_onmousedown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_AUX);
      return 0;
    case WM_LBUTTONUP:
      window_onmouseup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_LEFT);
      return 0;
    case WM_RBUTTONUP:
      window_onmouseup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_MBUTTONUP:
      window_onmousedown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_XBUTTONUP:
      window_onmouseup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_BUTTON_AUX);
      return 0;
    case WM_LBUTTONDBLCLK:
      window_dblclick();
      return 0;
    case WM_MOUSEWHEEL:
      window_onscroll(GET_WHEEL_DELTA_WPARAM(wParam));
      return 0;
    case WM_SIZE:
      if (d3d_device && window_focus) {
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
export void _window_onresize() {
  window_pixel_ndc[0] = 2.f / (f32)window_width;
  window_pixel_ndc[1] = 2.f / (f32)window_height;
  // d3d_render_target_view
  ID3D11Texture2D* back_buffer;
  HRESULT result = d3d_swapchain->lpVtbl->GetBuffer(
    d3d_swapchain, 0, &IID_ID3D11Texture2D,
    (void**)&back_buffer
  );
  if (FAILED(result)) {
    error("IDXGISwapChain_GetBuffer", result);
  }
  result = d3d_device->lpVtbl->CreateRenderTargetView(
    d3d_device, (ID3D11Resource*)back_buffer, 0, &d3d_render_target_view
  );
  back_buffer->lpVtbl->Release(back_buffer);
  if (FAILED(result)) {
    error("CreateRenderTargetView", result);
  }
  d3d_device_context->lpVtbl->OMSetRenderTargets(
    d3d_device_context, 1, &d3d_render_target_view, 0
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
  d3d_device_context->lpVtbl->RSSetViewports(
    d3d_device_context, 1, &viewport
  );
}
export void _renderer_thread() {
  f64 time = time_now_f64();
  // set multimedia thread
  DWORD task_index = 0;
  HANDLE mmtask = AvSetMmThreadCharacteristicsA("Games", &task_index);
  if (!mmtask) {
    console_log("Error: _renderer_thread AvSetMmThreadCharacteristicsA %x", GetLastError());
  }
  // timer
  HANDLE timer = CreateWaitableTimerA(0, false, 0);
  const LARGE_INTEGER due_time = { 0 };
  if (!SetWaitableTimer(timer, &due_time, 15, 0, 0, false)) {
    console_log("Error: _renderer_thread SetWaitableTimer %x", GetLastError());
  }
  timeBeginPeriod(1);
  // loop
  while (_renderer_thread_id) {
    WaitForSingleObject(timer, INFINITE);
    // timer
    const f64 now = time_now_f64();
    window_deltatime = now - time;
    time = now;
    if (!window_focus)
      continue;
#ifdef DEBUG
    const f64 frame_time = 1. / 60;
    if (window_deltatime > frame_time) {
      console_log("FPS DROP %f %f", frame_time, window_deltatime);
    }
#endif
    // onresize
    if (window_resized) {
      window_resized = false;
      d3d_render_target_view->lpVtbl->Release(d3d_render_target_view);
      HRESULT result = d3d_swapchain->lpVtbl->ResizeBuffers(
        d3d_swapchain, 1, window_width, window_height,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0
      );
      if (FAILED(result)) {
        error("IDXGISwapChain_ResizeBuffers", result);
      }
      _window_onresize();
      window_onresize();
      window_updated = true;
    }
    // frame
    if (window_updated) {
      window_updated = false;
      vertices_length = 0;
      indexes_length = 0;
      window_onrender();
      assert(vertices_length <= vertices_capacity);
      assert(indexes_length <= indexes_capacity);
      D3D11_MAPPED_SUBRESOURCE subresource;
      // vertices
      d3d_device_context->lpVtbl->Map(
        d3d_device_context, (ID3D11Resource*)vertices_buffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &subresource
      );
      memory_copy(subresource.pData, vertices_virtual, vertices_length * sizeof(vertex_t));
      d3d_device_context->lpVtbl->Unmap(
        d3d_device_context, (ID3D11Resource*)vertices_buffer, 0
      );
      // indexes
      d3d_device_context->lpVtbl->Map(
        d3d_device_context, (ID3D11Resource*)indexes_buffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &subresource
      );
      memory_copy(subresource.pData, indexes_virtual, indexes_length * sizeof(u32));
      d3d_device_context->lpVtbl->Unmap(
        d3d_device_context, (ID3D11Resource*)indexes_buffer, 0
      );
      // draw
      d3d_device_context->lpVtbl->ClearRenderTargetView(
        d3d_device_context, d3d_render_target_view, window_background
      );
      d3d_device_context->lpVtbl->DrawIndexed(d3d_device_context, indexes_length, 0, 0);
      d3d_swapchain->lpVtbl->Present(d3d_swapchain, 0, 0);
    }
  }
  // cleanup
  CloseHandle(timer);
  timeEndPeriod(1);
  AvRevertMmThreadCharacteristics(mmtask);
  assert(indexes_capacity > 0);
  _indexes_free();
  assert(vertices_capacity > 0);
  _vertices_free();
  d3d_blend_state->lpVtbl->Release(d3d_blend_state);
  d3d_sampler_state->lpVtbl->Release(d3d_sampler_state);
  d3d_pixel_shader->lpVtbl->Release(d3d_pixel_shader);
  d3d_vertex_shader->lpVtbl->Release(d3d_vertex_shader);
  d3d_input_layout->lpVtbl->Release(d3d_input_layout);
  d3d_rasterizer->lpVtbl->Release(d3d_rasterizer);
  d3d_render_target_view->lpVtbl->Release(d3d_render_target_view);
  d3d_device_context->lpVtbl->Release(d3d_device_context);
  d3d_device->lpVtbl->Release(d3d_device);
  d3d_swapchain->lpVtbl->Release(d3d_swapchain);
}
export void _d3d_buffer(ID3D11Buffer** ppBuffer, u64 ByteWidth, UINT BindFlags) {
  const D3D11_BUFFER_DESC buffer_desc = {
    .Usage = D3D11_USAGE_DYNAMIC,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    .ByteWidth = ByteWidth,
    .BindFlags = BindFlags
  };
  HRESULT result = d3d_device->lpVtbl->CreateBuffer(
    d3d_device, &buffer_desc, 0, ppBuffer
  );
  if (FAILED(result)) {
    error("CreateBuffer", result);
  }
}
export void window_close() { DestroyWindow(window_id); }
export void window_startup(const char* title, const char* atlas_path) {
  SetProcessDPIAware();
  i32 result;
  // window_class_register
  WNDCLASSEXA window_class = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = _window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .lpszClassName = title,
  };
  result = RegisterClassExA(&window_class);
  if (!result) {
    error("RegisterClassExA", result);
  }
  u32 window_style = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindowExA
  RECT rect = { 0, 0, window_width, window_height };
  AdjustWindowRectEx(&rect, window_style, false, window_ex_style);
  window_id = CreateWindowExA(
    window_ex_style, window_class.lpszClassName, title, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    window_class.hInstance,
    0 // pointer to window-creation data
  );
  if (!window_id) {
    error("CreateWindowExA", (error_t)window_id);
  }
  // global_d3d_device | global_d3d_swapchain | global_d3d_device_context
  DXGI_SWAP_CHAIN_DESC swapchain_desc = {
    .BufferCount = 1,                                // one back buffer
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
    .BufferDesc.Width = window_width,                // set the back buffer width
    .BufferDesc.Height = window_height,              // set the back buffer height
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,  // how swap chain is to be used
    .OutputWindow = window_id,                       // the window to be used
    .SampleDesc.Count = 4,                           // how many multisamples
    .Windowed = true,                                // windowed/full-screen mode
    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH  // allow full-screen switching
  };
  result = D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0, 0, 0, 0,
    D3D11_SDK_VERSION,
    &swapchain_desc,
    &d3d_swapchain,
    &d3d_device,
    0,
    &d3d_device_context
  );
  if (FAILED(result)) {
    error("D3D11CreateDeviceAndSwapChain", result);
  }
  d3d_device_context->lpVtbl->IASetPrimitiveTopology(d3d_device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _window_onresize();
  // global_d3d_rasterizer
  D3D11_RASTERIZER_DESC rasterizer_props = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE
  };
  result = d3d_device->lpVtbl->CreateRasterizerState(
    d3d_device, &rasterizer_props, &d3d_rasterizer
  );
  if (FAILED(result)) {
    error("ID3D11Device_CreateRasterizerState", result);
  }
  // vertex_shader
  const u8 vs_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x12, 0x63, 0xab, 0x0a, 0x71, 0x74, 0xd2, 0x4c, 0x65, 0x97, 0xf2, 0x8e, 0x53, 0x53, 0xb4, 0x88, 0x01, 0x00, 0x00, 0x00, 0x88, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0xac, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x58, 0x01, 0x00, 0x00, 0xec, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xfe, 0xff, 0x00, 0x01, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x4c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0x8c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0x23, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5f, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08, 0xc2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x36, 0x00, 0x00, 0x05, 0xf2, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  result = d3d_device->lpVtbl->CreateVertexShader(
    d3d_device,
    vs_cso,
    sizeof(vs_cso),
    0,
    &d3d_vertex_shader
  );
  if (FAILED(result)) {
    error("CreateVertexShader", result);
  }
  d3d_device_context->lpVtbl->VSSetShader(d3d_device_context, d3d_vertex_shader, NULL, 0);
  // texture_pixel_shader
  const u8 ps_cso [] = { 0x44, 0x58, 0x42, 0x43, 0x4d, 0x49, 0x7a, 0x88, 0x11, 0x67, 0x10, 0x16, 0x1e, 0xc7, 0xf2, 0x08, 0x81, 0xb5, 0x0e, 0xba, 0x01, 0x00, 0x00, 0x00, 0xec, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x5c, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x50, 0x02, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x05, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x73, 0x74, 0x61, 0x74, 0x65, 0x00, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x5f, 0x32, 0x64, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0xab, 0xab, 0xab, 0x49, 0x53, 0x47, 0x4e, 0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0xab, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0xb8, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x07, 0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x8b, 0xc2, 0x00, 0x00, 0x80, 0x43, 0x55, 0x15, 0x00, 0xf2, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x7e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x09, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x94, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  result = d3d_device->lpVtbl->CreatePixelShader(
    d3d_device,
    ps_cso,
    sizeof(ps_cso),
    0,
    &d3d_pixel_shader
  );
  if (FAILED(result)) {
    error("CreatePixelShader", result);
  }
  d3d_device_context->lpVtbl->PSSetShader(d3d_device_context, d3d_pixel_shader, NULL, 0);
  // texture_input_layout
  D3D11_INPUT_ELEMENT_DESC texture_layout [] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  const u8 texture_layout_size = sizeof(texture_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  result = d3d_device->lpVtbl->CreateInputLayout(
    d3d_device,
    texture_layout,
    texture_layout_size,
    vs_cso,
    sizeof(vs_cso),
    &d3d_input_layout
  );
  if (FAILED(result)) {
    error("CreateInputLayout", result);
  }
  d3d_device_context->lpVtbl->IASetInputLayout(d3d_device_context, d3d_input_layout);
  // sampler_state
  const D3D11_SAMPLER_DESC sampler_desc = {
    .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
    .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
    .ComparisonFunc = D3D11_COMPARISON_NEVER,
    .MinLOD = 0,
    .MinLOD = D3D11_FLOAT32_MAX
  };
  result = d3d_device->lpVtbl->CreateSamplerState(
    d3d_device, &sampler_desc, &d3d_sampler_state
  );
  if (FAILED(result)) {
    error("CreateSamplerState", result);
  }
  d3d_device_context->lpVtbl->PSSetSamplers(d3d_device_context, 0, 1, &d3d_sampler_state);
  // d3d_blend_state
  const D3D11_BLEND_DESC blend_desc = {
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
  d3d_device->lpVtbl->CreateBlendState(
    d3d_device, &blend_desc, &d3d_blend_state
  );
  const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
  d3d_device_context->lpVtbl->OMSetBlendState(
    d3d_device_context, d3d_blend_state, blend_factor, 0xffffffff
  );
  // load_image
  const u64 image_size = atlas_width * atlas_height * 4; // RGBA8
  const u32 image_stride = atlas_width * 4;
  FILE* file = fopen(atlas_path, "r");
  u8 image_data[image_size];
  fread(image_data, 1, image_size, file);
  fclose(file);
  // CreateTexture2D
  D3D11_TEXTURE2D_DESC texture_desc = {
    .Width = atlas_width,
    .Height = atlas_height,
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
  result = d3d_device->lpVtbl->CreateTexture2D(
    d3d_device, &texture_desc, &subresource_data, &texture
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
  result = d3d_device->lpVtbl->CreateShaderResourceView(
    d3d_device, (ID3D11Resource*)texture, &srv_desc, &d3d_shader_resource
  );
  if (FAILED(result)) {
    error("CreateShaderResourceView", result);
  }
  // PSSetShaderResources
  d3d_device_context->lpVtbl->PSSetShaderResources(d3d_device_context, 0, 1, &d3d_shader_resource);
}
export void vertices_reserve(u64 size) {
  if (size == 0) {
    return _vertices_free();
  }
  const u64 buffer_size = size * sizeof(vertex_t);
  if (vertices_capacity != 0) {
    void* buffer = memory_realloc(vertices_virtual, buffer_size);
    if (!buffer) {
      error("vertices_alloc memory_realloc", ERR_NOT_ENOUGH_MEMORY);
      return;
    }
    vertices_virtual = buffer;
    vertices_buffer->lpVtbl->Release(vertices_buffer);
  } else {
    vertices_virtual = memory_alloc(buffer_size);
    if (!vertices_virtual) {
      error("vertices_alloc memory_alloc", ERR_NOT_ENOUGH_MEMORY);
      return;
    }
  }
  vertices_capacity = size;
  _d3d_buffer(&vertices_buffer, buffer_size, D3D11_BIND_VERTEX_BUFFER);
  const u32 vertex_stride = sizeof(vertex_t);
  const u32 vertex_offset = 0;
  d3d_device_context->lpVtbl->IASetVertexBuffers(
    d3d_device_context, 0, 1, &vertices_buffer, &vertex_stride,
    &vertex_offset
  );
}
export void indexes_reserve(u64 size) {
  if (size == 0) {
    return _indexes_free();
  }
  const u64 buffer_size = size * sizeof(u32);
  if (indexes_capacity != 0) {
    void* buffer = memory_realloc(indexes_virtual, buffer_size);
    if (!buffer) {
      error("indexes_alloc memory_realloc", ERR_NOT_ENOUGH_MEMORY);
      return;
    }
    indexes_virtual = buffer;
    indexes_buffer->lpVtbl->Release(indexes_buffer);
  } else {
    indexes_virtual = memory_alloc(buffer_size);
    if (!indexes_virtual) {
      error("indexes_alloc memory_alloc", ERR_NOT_ENOUGH_MEMORY);
      return;
    }
  }
  indexes_capacity = size;
  _d3d_buffer(&indexes_buffer, buffer_size, D3D11_BIND_INDEX_BUFFER);
  d3d_device_context->lpVtbl->IASetIndexBuffer(d3d_device_context, indexes_buffer, DXGI_FORMAT_R32_UINT, 0);
}
export void window_set_title(const char* title) {
  assert(window_id != 0);
  SetWindowTextA(window_id, title);
}
export void window_run() {
  // renderer
  assert(_renderer_thread_id == 0);
  _renderer_thread_id = thread_new(_renderer_thread, 0);
  // loop
  SetTimer(0, 0, 0, (TIMERPROC)_window_onupdate);
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
        thread_free(_renderer_thread_id);
        _renderer_thread_id = 0;
        return;
      }
    }
  } while (true);
}

#endif