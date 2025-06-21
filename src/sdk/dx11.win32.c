#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <d3d11.h>
#include <avrt.h>

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
ID3D11Buffer* _d3d_buffer;

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
  }
  _vertices_length = 0;
  _indexes_length = 0;
  // buffer_map
  static D3D11_MAPPED_SUBRESOURCE subresource = {};
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
    _d3d_device_context, _d3d_render_target_view, (FLOAT*)&window_background
  );
  _d3d_device_context->lpVtbl->DrawIndexed(_d3d_device_context, _indexes_length, 0, 0);
  _d3d_swapchain->lpVtbl->Present(_d3d_swapchain, 0, 0);
}
void _gfx_inicialize(const char* atlas_path) {
// global_d3d_device | global_d3d_swapchain | global_d3d_device_context
  const DXGI_SWAP_CHAIN_DESC swapchain_desc = {
    .BufferCount = 1,                                // one back buffer
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color
    .BufferDesc.Width = window_width,                // set the back buffer width
    .BufferDesc.Height = window_height,              // set the back buffer height
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,  // how swap chain is to be used
    .OutputWindow = _window_id,                      // the window to be used
    .SampleDesc.Count = 4,                           // how many multisamples
    .Windowed = true,                                // windowed/full-screen mode
    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH  // allow full-screen switching
  };
  i32 result = D3D11CreateDeviceAndSwapChain(
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
  // ID3D11RasterizerState
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
  i32 file_size;
  u8* file_bytes = fs_readfile_sync("share/ps.cso", &file_size);
  result = _d3d_device->lpVtbl->CreatePixelShader(
    _d3d_device,
    file_bytes,
    file_size,
    0,
    &_d3d_pixel_shader
  );
  if (FAILED(result)) {
    error(result, "CreatePixelShader");
    goto pixel_shader_free;
  }
  _d3d_device_context->lpVtbl->PSSetShader(_d3d_device_context, _d3d_pixel_shader, NULL, 0);
pixel_shader_free:
  memory_free(file_bytes);
  // vertex_shader
  file_bytes = fs_readfile_sync("share/vs.cso", &file_size);
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
  // input_layout
  const D3D11_INPUT_ELEMENT_DESC texture_layout [] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  result = _d3d_device->lpVtbl->CreateInputLayout(
    _d3d_device,
    texture_layout,
    sizeof(texture_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
    file_bytes,
    file_size,
    &_d3d_input_layout
  );
  if (FAILED(result)) {
    error(result, "CreateInputLayout");
    goto vertex_shader_free;
  }
  _d3d_device_context->lpVtbl->IASetInputLayout(_d3d_device_context, _d3d_input_layout);
vertex_shader_free:
  memory_free(file_bytes);
  // sampler_state
  const D3D11_SAMPLER_DESC sampler_desc = {
    .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
    .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
    .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
    .ComparisonFunc = D3D11_COMPARISON_NEVER,
    .MinLOD = 0,
    .MaxLOD = D3D11_FLOAT32_MAX
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
  if (atlas_path) {
    // atlas_load
    file_bytes = fs_readfilen_sync(atlas_path, atlas_width * atlas_height * 4);
    if (file_bytes == null) {
      error(ERR_NOT_FOUND, "atlas_load");
      goto atlas_exit;
    }
    // CreateTexture2D
    const D3D11_TEXTURE2D_DESC texture_desc = {
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
    const D3D11_SUBRESOURCE_DATA subresource_data = {
      .pSysMem = file_bytes,
      .SysMemPitch = atlas_width * 4
    };
    ID3D11Texture2D* texture;
    result = _d3d_device->lpVtbl->CreateTexture2D(
      _d3d_device, &texture_desc, &subresource_data, &texture
    );
    if (FAILED(result)) {
      error(result, "CreateTexture2D");
      goto atlas_free;
    }
    // CreateShaderResourceView
    const D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
      .Format = texture_desc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D.MipLevels = 1
    };
    result = _d3d_device->lpVtbl->CreateShaderResourceView(
      _d3d_device, (ID3D11Resource*)texture, &srv_desc, &_d3d_shader_resource
    );
    if (FAILED(result)) {
      error(result, "CreateShaderResourceView");
      goto atlas_free;
    }
    // PSSetShaderResources
    _d3d_device_context->lpVtbl->PSSetShaderResources(_d3d_device_context, 0, 1, &_d3d_shader_resource);
  atlas_free:
    memory_free(file_bytes);
  atlas_exit:
  }
}
void _gfx_destroy() {
  _d3d_buffer->lpVtbl->Release(_d3d_buffer);
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
}
void vertices_reserve(u64 vertices_size, u64 indexes_size) {
  if (vertices_size == 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
    vertices_size = 0;
    return;
  }
  const u64 vertices_buffer_size = vertices_size * sizeof(vertex_t);
  const u64 indexes_buffer_size = indexes_size * sizeof(u32);
  if (vertices_capacity != 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
  }
  // d3d_buffers
  HRESULT result;
  D3D11_BUFFER_DESC buffer_desc = {
    .Usage = D3D11_USAGE_DYNAMIC,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    .BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER,
    .ByteWidth = vertices_buffer_size + indexes_buffer_size
  };
  const u32 offset = 0;
  // d3d_vertices_buffer
  result = _d3d_device->lpVtbl->CreateBuffer(
    _d3d_device, &buffer_desc, 0, &_d3d_buffer
  );
  if (FAILED(result)) {
    error(result, "vertices_reserve vertices CreateBuffer");
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