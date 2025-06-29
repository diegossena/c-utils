#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <d3d11.h>
#include <avrt.h>

// 546760 bytes

HWND _window_id;
ID3D11Device* _d3d_device;
IDXGISwapChain* _d3d_swapchain;
ID3D11DeviceContext* _d3d_device_context;
ID3D11RenderTargetView* _d3d_render_target_view;
ID3D11Buffer* _d3d_buffer;

void _window_resize() {
  ID3D11Texture2D* rtv_texture;
  HRESULT result = _d3d_swapchain->lpVtbl->GetBuffer(
    _d3d_swapchain, 0, &IID_ID3D11Texture2D,
    (void**)&rtv_texture
  );
  if (FAILED(result)) {
    error(result, "IDXGISwapChain_GetBuffer");
  }
  result = _d3d_device->lpVtbl->CreateRenderTargetView(
    _d3d_device, (ID3D11Resource*)rtv_texture, 0, &_d3d_render_target_view
  );
  rtv_texture->lpVtbl->Release(rtv_texture);
  if (FAILED(result)) {
    error(result, "CreateRenderTargetView");
  }
  _d3d_device_context->lpVtbl->OMSetRenderTargets(
    _d3d_device_context, 1, &_d3d_render_target_view, 0
  );
  const D3D11_VIEWPORT viewport = {
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
void _gfx_render() {
  timeBeginPeriod(1);
  DWORD task_index = 0;
  HANDLE mmtask = AvSetMmThreadCharacteristicsA("Games", &task_index);
  if (!mmtask) {
    error(GetLastError(), "window_run AvSetMmThreadCharacteristicsA");
  }
  HANDLE timer = CreateWaitableTimerA(0, false, 0);
  const LARGE_INTEGER due_time = { 0 };
  if (!SetWaitableTimer(timer, &due_time, 15, 0, 0, false)) {
    error(GetLastError(), "_gfx_render SetWaitableTimer");
  }
  f64 timer_ticks = time_ticks();
  do {
    WaitForSingleObject(timer, INFINITE);
    // onresize
    if (window_resized) {
      window_resized = false;
      _d3d_render_target_view->lpVtbl->Release(_d3d_render_target_view);
      HRESULT result = _d3d_swapchain->lpVtbl->ResizeBuffers(
        _d3d_swapchain, 0, window_width, window_height,
        DXGI_FORMAT_UNKNOWN, 0
      );
      if (FAILED(result)) {
        error(result, "IDXGISwapChain_ResizeBuffers");
      }
      _window_resize();
      window_onresize();
    }
    const f64 ticks = time_ticks();
    window_deltatime = ticks - timer_ticks;
    timer_ticks = ticks;
#ifdef DEBUG
    if (window_deltatime > 1. / 60) {
      console_log("FPS DROP %f %f", 1. / 60, window_deltatime);
    }
#endif
    _vertices_length = 0;
    _indexes_length = 0;
    // buffer_map
    D3D11_MAPPED_SUBRESOURCE subresource;
    _d3d_device_context->lpVtbl->Map(
      _d3d_device_context, (ID3D11Resource*)_d3d_buffer, 0,
      D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource
    );
    _vertices_virtual = subresource.pData;
    _indexes_virtual = subresource.pData + vertices_capacity * sizeof(vertex_t);
    window_onrender();
    _d3d_device_context->lpVtbl->Unmap(
      _d3d_device_context, (ID3D11Resource*)_d3d_buffer, 0
    );
    assert(_vertices_length <= vertices_capacity);
    assert(_indexes_length <= indexes_capacity);
    // draw
    _d3d_device_context->lpVtbl->ClearRenderTargetView(
      _d3d_device_context, _d3d_render_target_view, (FLOAT*)&background_color
    );
    _d3d_device_context->lpVtbl->DrawIndexed(_d3d_device_context, _indexes_length, 0, 0);
    _d3d_swapchain->lpVtbl->Present(_d3d_swapchain, 1, 0);
  } while (_window_id);
  timeEndPeriod(1);
  AvRevertMmThreadCharacteristics(mmtask);
  _d3d_buffer->lpVtbl->Release(_d3d_buffer);
  _d3d_render_target_view->lpVtbl->Release(_d3d_render_target_view);
  _d3d_device_context->lpVtbl->Release(_d3d_device_context);
  _d3d_device->lpVtbl->Release(_d3d_device);
  _d3d_swapchain->lpVtbl->Release(_d3d_swapchain);
}
void _gfx_startup(const char* atlas_path) {
  i32 file_size;
  u8* file_bytes;
  i32 result;
  { // D3D11CreateDeviceAndSwapChain
    const DXGI_SWAP_CHAIN_DESC swapchain_desc = {
      .BufferCount = 1,                                // one back buffer
      .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
      .BufferDesc.Width = window_width,                // set the back buffer width
      .BufferDesc.Height = window_height,              // set the back buffer height
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,  // how swap chain is to be used
      .OutputWindow = _window_id,                      // the window to be used
      .SampleDesc.Count = 1,                           // how many multisamples
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
      exit(result);
    }
  }
  _d3d_device_context->lpVtbl->IASetPrimitiveTopology(_d3d_device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  { // CreatePixelShader
    u8* ps_data = fs_readfile_sync("share/ps.cso", &file_size);
    ID3D11PixelShader* pixel_shader;
    result = _d3d_device->lpVtbl->CreatePixelShader(
      _d3d_device,
      ps_data,
      file_size,
      0,
      &pixel_shader
    );
    if (FAILED(result)) {
      error(result, "CreatePixelShader");
      exit(result);
    }
    _d3d_device_context->lpVtbl->PSSetShader(_d3d_device_context, pixel_shader, null, 0);
    pixel_shader->lpVtbl->Release(pixel_shader);
    memory_free(ps_data);
  }
  { // ID3D11VertexShader
    ID3D11VertexShader* vertex_shader;
    file_bytes = fs_readfile_sync("share/vs.cso", &file_size);
    result = _d3d_device->lpVtbl->CreateVertexShader(
      _d3d_device,
      file_bytes,
      file_size,
      0,
      &vertex_shader
    );
    if (FAILED(result)) {
      error(result, "CreateVertexShader");
    }
    _d3d_device_context->lpVtbl->VSSetShader(_d3d_device_context, vertex_shader, NULL, 0);
    vertex_shader->lpVtbl->Release(vertex_shader);
    // ID3D11InputLayout
    const D3D11_INPUT_ELEMENT_DESC texture_layout [] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    ID3D11InputLayout* _d3d_input_layout;
    result = _d3d_device->lpVtbl->CreateInputLayout(
      _d3d_device,
      texture_layout,
      sizeof(texture_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
      file_bytes,
      file_size,
      &_d3d_input_layout
    );
    memory_free(file_bytes);
    if (FAILED(result)) {
      error(result, "CreateInputLayout");
      exit(result);
    }
    _d3d_device_context->lpVtbl->IASetInputLayout(_d3d_device_context, _d3d_input_layout);
    _d3d_input_layout->lpVtbl->Release(_d3d_input_layout);
  }
  { // ID3D11SamplerState
    const D3D11_SAMPLER_DESC sampler_desc = {
      .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
      .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
      .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
      .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
      .ComparisonFunc = D3D11_COMPARISON_NEVER,
      .MaxLOD = D3D11_FLOAT32_MAX
    };
    ID3D11SamplerState* sampler;
    result = _d3d_device->lpVtbl->CreateSamplerState(
      _d3d_device, &sampler_desc, &sampler
    );
    if (FAILED(result)) {
      error(result, "CreateSamplerState");
    }
    _d3d_device_context->lpVtbl->PSSetSamplers(_d3d_device_context, 0, 1, &sampler);
    sampler->lpVtbl->Release(sampler);
  }
  { // ID3D11BlendState
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
    ID3D11BlendState* blend_state;
    _d3d_device->lpVtbl->CreateBlendState(
      _d3d_device, &blend_desc, &blend_state
    );
    const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    _d3d_device_context->lpVtbl->OMSetBlendState(
      _d3d_device_context, blend_state, blend_factor, 0xffffffff
    );
    blend_state->lpVtbl->Release(blend_state);
  }
  { // atlas_load
    file_bytes = fs_readfilen_sync(atlas_path, ATLAS_WIDTH * ATLAS_HEIGHT * 4);
    if (file_bytes == null) {
      error(ERR_NOT_FOUND, "atlas_load");
      exit(result);
    }
    // CreateTexture2D
    const D3D11_TEXTURE2D_DESC texture_desc = {
      .Width = ATLAS_WIDTH,
      .Height = ATLAS_HEIGHT,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc.Count = 1,
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_SHADER_RESOURCE,
    };
    const D3D11_SUBRESOURCE_DATA subresource_data = {
      .pSysMem = file_bytes,
      .SysMemPitch = ATLAS_WIDTH * 4
    };
    ID3D11Texture2D* texture;
    result = _d3d_device->lpVtbl->CreateTexture2D(
      _d3d_device, &texture_desc, &subresource_data, &texture
    );
    if (FAILED(result)) {
      error(result, "CreateTexture2D");
      exit(result);
    }
    // CreateShaderResourceView
    const D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
      .Format = texture_desc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D.MipLevels = 1
    };
    ID3D11ShaderResourceView* resource_view;
    result = _d3d_device->lpVtbl->CreateShaderResourceView(
      _d3d_device, (ID3D11Resource*)texture, &srv_desc, &resource_view
    );
    if (FAILED(result)) {
      error(result, "CreateShaderResourceView");
      exit(result);
    }
    // PSSetShaderResources
    _d3d_device_context->lpVtbl->PSSetShaderResources(_d3d_device_context, 0, 1, &resource_view);
    resource_view->lpVtbl->Release(resource_view);
    memory_free(file_bytes);
  }
  _window_resize();
}
void vertices_reserve(u64 vertices_size, u64 indexes_size) {
  if (vertices_size == 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
    vertices_size = 0;
    return;
  } else if (vertices_capacity != 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
  }
  const u64 vertices_buffer_size = vertices_size * sizeof(vertex_t);
  const u64 indexes_buffer_size = indexes_size * sizeof(u32);
  // d3d_buffer
  const D3D11_BUFFER_DESC buffer_desc = {
    .Usage = D3D11_USAGE_DYNAMIC,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    .BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER,
    .ByteWidth = vertices_buffer_size + indexes_buffer_size
  };
  const u32 offset = 0;
  // d3d_vertices_buffer
  HRESULT result = _d3d_device->lpVtbl->CreateBuffer(
    _d3d_device, &buffer_desc, 0, &_d3d_buffer
  );
  if (FAILED(result)) {
    error(result, "vertices_reserve CreateBuffer");
  }
  const u32 stride = sizeof(vertex_t);
  _d3d_device_context->lpVtbl->IASetVertexBuffers(
    _d3d_device_context, 0, 1, &_d3d_buffer, &stride,
    &offset
  );
  _d3d_device_context->lpVtbl->IASetIndexBuffer(
    _d3d_device_context, _d3d_buffer, DXGI_FORMAT_R32_UINT,
    vertices_buffer_size
  );
  // update
  vertices_capacity = vertices_size;
  indexes_capacity = indexes_size;
}

#endif