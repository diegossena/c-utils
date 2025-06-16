#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <avrt.h>
// FILE, fopen, fclose, fseek, fclose, ftell
#include <stdio.h>

HWND _window_id;
f64 _window_render_time;
ID3D11Device* _d3d_device;
IDXGISwapChain* _d3d_swapchain;
ID3D11DeviceContext* _d3d_device_context;
ID3D11RasterizerState* _d3d_rasterizer;
ID3D11RenderTargetView* _d3d_render_target_view;
ID3D11InputLayout* _d3d_input_layout;
ID3D11VertexShader* _d3d_vertex_shader;
ID3D11PixelShader* _d3d_pixel_shader;
ID3D11SamplerState* _d3d_sampler_state;
ID3D11BlendState* _d3d_blend_state;
ID3D11ShaderResourceView* _d3d_shader_resource;
ID3D11Buffer* _d3d_vertices_buffer;
ID3D11Buffer* _d3d_indexes_buffer;
vertex_t* _vertices_virtual;
u32* _indexes_virtual;

void _vertices_free() {
  assert(indexes_capacity > 0);
  assert(vertices_capacity > 0);
  _d3d_vertices_buffer->lpVtbl->Release(_d3d_vertices_buffer);
  memory_free(_vertices_virtual);
  _d3d_vertices_buffer->lpVtbl->Release(_d3d_vertices_buffer);
  memory_free(_indexes_virtual);
}
void _window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (window_focus && _keyboard_count) {
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
          _keyboard_state[byte_index] |= (1 << bit_index);
          ++_keyboard_count;
          window_onkeydown(wParam);
        }
      }
      return 0;
    case WM_KEYUP:
      if (window_focus) {
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        _keyboard_state[byte_index] &= ~(1 << bit_index);
        --_keyboard_count;
        window_onkeyup(wParam);
      }
      return 0;
    case WM_MOUSEMOVE:
      window_onmousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
      window_dblclick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_MOUSEWHEEL:
      window_onscroll(GET_WHEEL_DELTA_WPARAM(wParam));
      return 0;
    case WM_SIZE:
      if (_d3d_device && window_focus) {
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
void _window_resize() {
  window_ndc_x = 2.f / (f32)window_width;
  window_ndc_y = 2.f / (f32)window_height;
  // d3d_render_target_view
  ID3D11Texture2D* texture;
  HRESULT result = _d3d_swapchain->lpVtbl->GetBuffer(
    _d3d_swapchain, 0, &IID_ID3D11Texture2D,
    (void**)&texture
  );
  if (FAILED(result)) {
    error(result, "IDXGISwapChain_GetBuffer");
  }
  result = _d3d_device->lpVtbl->CreateRenderTargetView(
    _d3d_device, (ID3D11Resource*)texture, 0, &_d3d_render_target_view
  );
  texture->lpVtbl->Release(texture);
  if (FAILED(result)) {
    error(result, "CreateRenderTargetView");
  }
  _d3d_device_context->lpVtbl->OMSetRenderTargets(
    _d3d_device_context, 1, &_d3d_render_target_view, 0
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
  _d3d_device_context->lpVtbl->RSSetViewports(
    _d3d_device_context, 1, &viewport
  );
}
void _window_render() {
  const f64 now = time_now_f64();
  const f64 delta_time = now - _window_render_time;
  window_deltatime = delta_time;
  _window_render_time = now;
  if (!window_focus)
    return;
#ifdef DEBUG
  const f64 frame_time = 1. / 60;
  if (window_deltatime > frame_time) {
    console_log("FPS DROP %f %f", frame_time, window_deltatime);
  }
#endif
  // onresize
  if (window_resized) {
    window_resized = false;
    _d3d_render_target_view->lpVtbl->Release(_d3d_render_target_view);
    HRESULT result = _d3d_swapchain->lpVtbl->ResizeBuffers(
      _d3d_swapchain, 1, window_width, window_height,
      DXGI_FORMAT_R8G8B8A8_UNORM, 0
    );
    if (FAILED(result)) {
      error(result, "IDXGISwapChain_ResizeBuffers");
    }
    _window_resize();
    window_onresize();
    window_updated = true;
  }
  if (window_updated) {
    // render
    window_updated = false;
    _vertices_length = 0;
    _indexes_length = 0;
    window_onrender();
    assert(_vertices_length <= vertices_capacity);
    assert(_indexes_length <= indexes_capacity);
    static D3D11_MAPPED_SUBRESOURCE subresource = {};
    // vertices_map
    _d3d_device_context->lpVtbl->Map(
      _d3d_device_context, (ID3D11Resource*)_d3d_vertices_buffer, 0,
      D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource
    );
    memory_copy(subresource.pData, _vertices_virtual, _vertices_length * sizeof(vertex_t));
    _d3d_device_context->lpVtbl->Unmap(
      _d3d_device_context, (ID3D11Resource*)_d3d_vertices_buffer, 0
    );
    // indexes_map
    _d3d_device_context->lpVtbl->Map(
      _d3d_device_context, (ID3D11Resource*)_d3d_indexes_buffer, 0,
      D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource
    );
    memory_copy(subresource.pData, _indexes_virtual, _indexes_length * sizeof(u32));
    _d3d_device_context->lpVtbl->Unmap(
      _d3d_device_context, (ID3D11Resource*)_d3d_indexes_buffer, 0
    );
    // draw
    _d3d_device_context->lpVtbl->ClearRenderTargetView(
      _d3d_device_context, _d3d_render_target_view, (FLOAT*)&window_background
    );
    _d3d_device_context->lpVtbl->DrawIndexed(_d3d_device_context, _indexes_length, 0, 0);
    _d3d_swapchain->lpVtbl->Present(_d3d_swapchain, 0, 0);
  }
}
void window_close() { DestroyWindow(_window_id); }
void window_startup(const char* title, const char* atlas_path) {
  i32 result;
  FILE* file;
  i32 file_size;
  u8* file_bytes = null;
  SetProcessDPIAware();
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
    error(result, "RegisterClassExA");
  }
  u32 window_style = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindowExA
  RECT rect = { 0, 0, window_width, window_height };
  AdjustWindowRectEx(&rect, window_style, false, window_ex_style);
  _window_id = CreateWindowExA(
    window_ex_style, window_class.lpszClassName, title, window_style,
    0, 0, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    window_class.hInstance,
    0 // pointer to window-creation data
  );
  if (!_window_id) {
    error((error_t)_window_id, "CreateWindowExA");
  }
  // global_d3d_device | global_d3d_swapchain | global_d3d_device_context
  const DXGI_SWAP_CHAIN_DESC swapchain_desc = {
    .BufferCount = 1,                                // one back buffer
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
    .BufferDesc.Width = window_width,                // set the back buffer width
    .BufferDesc.Height = window_height,              // set the back buffer height
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,  // how swap chain is to be used
    .OutputWindow = _window_id,                       // the window to be used
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
    &_d3d_swapchain,
    &_d3d_device,
    0,
    &_d3d_device_context
  );
  if (FAILED(result)) {
    error(result, "D3D11CreateDeviceAndSwapChain");
  }
  _d3d_device_context->lpVtbl->IASetPrimitiveTopology(_d3d_device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _window_resize();
  // global_d3d_rasterizer
  D3D11_RASTERIZER_DESC rasterizer_props = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE
  };
  result = _d3d_device->lpVtbl->CreateRasterizerState(
    _d3d_device, &rasterizer_props, &_d3d_rasterizer
  );
  if (FAILED(result)) {
    error(result, "ID3D11Device_CreateRasterizerState");
  }
    // pixel_shader
  file = fopen("share/ps.cso", "rb");
  if (file == null) {
    error(ERR_NOT_FOUND, "pixel_shader fopen");
    goto pixel_shader_exit;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    error(ERR_UNKNOWN, "pixel_shader fseek SEEK_END");
    goto pixel_shader_close;
  }
  file_size = ftell(file);
  if (file_size == -1) {
    error(ERR_UNKNOWN, "pixel_shader ftell");
    goto pixel_shader_close;
  }
  if (fseek(file, 0, SEEK_SET) != 0) {
    error(ERR_UNKNOWN, "pixel_shader ftell SEEK_SET");
    goto pixel_shader_close;
  }
  file_bytes = memory_alloc(file_size);
  if (file_bytes == null) {
    error(ERROR_NOT_ENOUGH_MEMORY, "pixel_shader memory_alloc");
    goto pixel_shader_close;
  }
  result = fread(file_bytes, sizeof(u8), file_size, file);
  console_log("pixel_shader file_size %d result %d", file_size, result);
  if (result != file_size) {
    error(ERR_UNKNOWN, "pixel_shader fread %d", result);
    goto pixel_shader_free;
  }
  result = _d3d_device->lpVtbl->CreatePixelShader(
    _d3d_device,
    file_bytes,
    file_size,
    0,
    &_d3d_pixel_shader
  );
  if (FAILED(result)) {
    error(result, "CreatePixelShader");
  }
  _d3d_device_context->lpVtbl->PSSetShader(_d3d_device_context, _d3d_pixel_shader, NULL, 0);
pixel_shader_free:
  memory_free(file_bytes);
pixel_shader_close:
  fclose(file);
pixel_shader_exit:
  // vertex_shader
  file = fopen("share/vs.cso", "rb");
  if (file == null) {
    error(ERR_NOT_FOUND, "vertex_shader fopen");
    goto vertex_shader_exit;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    error(ERR_UNKNOWN, "vertex_shader fseek");
    goto vertex_shader_close;
  }
  file_size = ftell(file);
  if (file_size == -1L) {
    error(ERR_UNKNOWN, "vertex_shader ftell");
    goto vertex_shader_close;
  }
  if (fseek(file, 0, SEEK_SET) != 0) {
    error(ERR_UNKNOWN, "vertex_shader ftell SEEK_SET");
    goto pixel_shader_close;
  }
  file_bytes = memory_alloc(file_size);
  if (file_bytes == null) {
    error(ERROR_NOT_ENOUGH_MEMORY, "vertex_shader memory_alloc");
    goto vertex_shader_close;
  }
  result = fread(file_bytes, sizeof(u8), file_size, file);
  if (result != file_size) {
    error(ERR_UNKNOWN, "vertex_shader fread %d", result);
    goto vertex_shader_free;
  }
  result = _d3d_device->lpVtbl->CreateVertexShader(
    _d3d_device,
    file_bytes,
    file_size,
    0,
    &_d3d_vertex_shader
  );
  if (FAILED(result)) {
    error(result, "CreateVertexShader");
  }
  _d3d_device_context->lpVtbl->VSSetShader(_d3d_device_context, _d3d_vertex_shader, NULL, 0);
  // texture_input_layout
  D3D11_INPUT_ELEMENT_DESC texture_layout [] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  const u8 texture_layout_size = sizeof(texture_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
  result = _d3d_device->lpVtbl->CreateInputLayout(
    _d3d_device,
    texture_layout,
    texture_layout_size,
    file_bytes,
    file_size,
    &_d3d_input_layout
  );
  if (FAILED(result)) {
    error(result, "CreateInputLayout");
  }
  _d3d_device_context->lpVtbl->IASetInputLayout(_d3d_device_context, _d3d_input_layout);
vertex_shader_free:
  memory_free(file_bytes);
vertex_shader_close:
  fclose(file);
vertex_shader_exit:
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
  result = _d3d_device->lpVtbl->CreateSamplerState(
    _d3d_device, &sampler_desc, &_d3d_sampler_state
  );
  if (FAILED(result)) {
    error(result, "CreateSamplerState");
  }
  _d3d_device_context->lpVtbl->PSSetSamplers(_d3d_device_context, 0, 1, &_d3d_sampler_state);
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
  _d3d_device->lpVtbl->CreateBlendState(
    _d3d_device, &blend_desc, &_d3d_blend_state
  );
  const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
  _d3d_device_context->lpVtbl->OMSetBlendState(
    _d3d_device_context, _d3d_blend_state, blend_factor, 0xffffffff
  );
  // load_image
  const u32 image_stride = atlas_width * 4;
  const u64 image_size = atlas_width * atlas_height * 4; // RGBA8
  file = fopen(atlas_path, "r");
  if (file == null) {
    error(ERR_NOT_FOUND, "atlas fopen");
    goto atlas_exit;
  }
  file_bytes = memory_alloc(image_size);
  if (file_bytes == null) {
    error(ERROR_NOT_ENOUGH_MEMORY, "atlas memory_alloc");
    goto atlas_close;
  }
  result = fread(file_bytes, sizeof(u8), image_size, file);
  if (result != image_size) {
    error(ERR_UNKNOWN, "atlas fread %d", result);
    goto atlas_free;
  }
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
    .pSysMem = file_bytes,
    .SysMemPitch = image_stride,
  };
  ID3D11Texture2D* texture;
  result = _d3d_device->lpVtbl->CreateTexture2D(
    _d3d_device, &texture_desc, &subresource_data, &texture
  );
  if (FAILED(result)) {
    error(result, "CreateTexture2D");
  }
atlas_free:
  memory_free(file_bytes);
atlas_close:
  fclose(file);
atlas_exit:
  // CreateShaderResourceView
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
    .Format = texture_desc.Format,
    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
    .Texture2D.MipLevels = 1
  };
  result = _d3d_device->lpVtbl->CreateShaderResourceView(
    _d3d_device, (ID3D11Resource*)texture, &srv_desc, &_d3d_shader_resource
  );
  if (FAILED(result)) {
    error(result, "CreateShaderResourceView");
  }
  // PSSetShaderResources
  _d3d_device_context->lpVtbl->PSSetShaderResources(_d3d_device_context, 0, 1, &_d3d_shader_resource);
}
void vertices_reserve(u64 vertices_size, u64 indexes_size) {
  if (vertices_size == 0) {
    _vertices_free();
    vertices_size = 0;
    return;
  }
  const u64 vertices_buffer_size = vertices_size * sizeof(vertex_t);
  const u64 indexes_buffer_size = indexes_size * sizeof(u32);
  // virtual
  if (vertices_capacity != 0) {
    // vertices_virtual_realloc
    void* buffer = memory_realloc(_vertices_virtual, vertices_buffer_size);
    if (!buffer) {
      error(ERR_NOT_ENOUGH_MEMORY, "vertices realloc");
      return;
    }
    _vertices_virtual = buffer;
    // indexes_virtual_realloc
    buffer = memory_realloc(_indexes_virtual, indexes_buffer_size);
    if (!buffer) {
      error(ERR_NOT_ENOUGH_MEMORY, "indexes realloc");
      return;
    }
    _vertices_virtual = buffer;
    // buffers_release
    _d3d_vertices_buffer->lpVtbl->Release(_d3d_vertices_buffer);
    _d3d_indexes_buffer->lpVtbl->Release(_d3d_indexes_buffer);
  } else {
    _vertices_virtual = memory_alloc(vertices_buffer_size);
    if (!_vertices_virtual) {
      error(ERR_NOT_ENOUGH_MEMORY, "vertices_alloc memory_alloc");
      return;
    }
    _indexes_virtual = memory_alloc(indexes_buffer_size);
    if (!_indexes_virtual) {
      error(ERR_NOT_ENOUGH_MEMORY, "indexes_alloc memory_alloc");
      return;
    }
  }
  // d3d_buffers
  HRESULT result;
  D3D11_BUFFER_DESC buffer_desc = {
    .Usage = D3D11_USAGE_DYNAMIC,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
  };
  const u32 offset = 0;
  // d3d_vertices_buffer
  buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  buffer_desc.ByteWidth = vertices_buffer_size;
  result = _d3d_device->lpVtbl->CreateBuffer(
    _d3d_device, &buffer_desc, 0, &_d3d_vertices_buffer
  );
  if (FAILED(result)) {
    error(result, "vertices_reserve vertices CreateBuffer");
  }
  const u32 stride = sizeof(vertex_t);
  _d3d_device_context->lpVtbl->IASetVertexBuffers(
    _d3d_device_context, 0, 1, &_d3d_vertices_buffer, &stride,
    &offset
  );
  // indexes_buffer
  buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  buffer_desc.ByteWidth = indexes_size * sizeof(u32);
  result = _d3d_device->lpVtbl->CreateBuffer(
    _d3d_device, &buffer_desc, 0, &_d3d_indexes_buffer
  );
  if (FAILED(result)) {
    error(result, "vertices_reserve indexes CreateBuffer");
  }
  _d3d_device_context->lpVtbl->IASetIndexBuffer(_d3d_device_context, _d3d_indexes_buffer, DXGI_FORMAT_R32_UINT, 0);
  // update
  vertices_capacity = vertices_size;
  indexes_capacity = indexes_size;
}
void window_set_title(const char* title) {
  assert(_window_id != 0);
  SetWindowTextA(_window_id, title);
}
void window_run() {
  // renderer
  timeBeginPeriod(1);
  DWORD task_index = 0;
  HANDLE mmtask = AvSetMmThreadCharacteristicsA("Games", &task_index);
  if (!mmtask) {
    error(GetLastError(), "window_run AvSetMmThreadCharacteristicsA");
  }
  HANDLE renderer_timer;
  _window_render_time = time_now_f64();
  CreateTimerQueueTimer(&renderer_timer, 0, (WAITORTIMERCALLBACK)_window_render, 0, 0, 15, 0);
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
        timeEndPeriod(1);
        KillTimer(0, 0);
        AvRevertMmThreadCharacteristics(mmtask);
        DeleteTimerQueueTimer(0, renderer_timer, INVALID_HANDLE_VALUE);
        _vertices_free();
        _d3d_blend_state->lpVtbl->Release(_d3d_blend_state);
        _d3d_sampler_state->lpVtbl->Release(_d3d_sampler_state);
        _d3d_pixel_shader->lpVtbl->Release(_d3d_pixel_shader);
        _d3d_vertex_shader->lpVtbl->Release(_d3d_vertex_shader);
        _d3d_input_layout->lpVtbl->Release(_d3d_input_layout);
        _d3d_rasterizer->lpVtbl->Release(_d3d_rasterizer);
        _d3d_render_target_view->lpVtbl->Release(_d3d_render_target_view);
        _d3d_device_context->lpVtbl->Release(_d3d_device_context);
        _d3d_device->lpVtbl->Release(_d3d_device);
        _d3d_swapchain->lpVtbl->Release(_d3d_swapchain);
        return;
      }
    }
  } while (true);
}

#endif