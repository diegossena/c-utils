#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/memory.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#define D3D_FRAME_COUNT 2
#define D3D_BUFFER_TARGET_STATE (D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER)

ID3D12Device* _d3d_device;
IDXGISwapChain3* _swapchain;
ID3D12Resource* _d3d_render_targets[D3D_FRAME_COUNT];
ID3D12Resource* _d3d_texture;
ID3D12CommandQueue* _d3d_command_queue;
ID3D12CommandAllocator* _command_allocator;
ID3D12GraphicsCommandList* _d3d_command_list;
ID3D12PipelineState* _pipeline_state;
ID3D12RootSignature* _d3d_root_signature;
ID3D12Resource* _d3d_buffer;
ID3D12Resource* _d3d_buffer_upload;
ID3D12DescriptorHeap* _rtv_heap;
u8 _d3d_rtv_descriptor_size;
D3D12_CPU_DESCRIPTOR_HANDLE _rtv_handle;
ID3D12DescriptorHeap* _srv_heap;
D3D12_GPU_DESCRIPTOR_HANDLE _srv_gpu_handle;

ID3D12Fence* _d3d_fence;
HANDLE _d3d_fence_event;
u64 _d3d_fence_value;

void _d3d_wait() {
  HRESULT result = _d3d_command_queue->lpVtbl->Signal(_d3d_command_queue, _d3d_fence, ++_d3d_fence_value);
  u64 completed = _d3d_fence->lpVtbl->GetCompletedValue(_d3d_fence);
  if (completed == -1) {
    result = _d3d_device->lpVtbl->GetDeviceRemovedReason(_d3d_device);
    switch (result) {
      case DXGI_ERROR_DEVICE_HUNG:
        error(result, "_d3d_gpu_wait GetCompletedValue DXGI_ERROR_DEVICE_HUNG");
        break;
    }
    exit(result);
  }
  if (completed < _d3d_fence_value) {
    _d3d_fence->lpVtbl->SetEventOnCompletion(_d3d_fence, _d3d_fence_value, _d3d_fence_event);
    WaitForSingleObject(_d3d_fence_event, INFINITE);
  }
}
void _d3d_reset() {
  i32 result = _command_allocator->lpVtbl->Reset(_command_allocator);
  if (FAILED(result)) {
    error(result, "_d3d_command_allocator Reset");
    exit(result);
  }
  result = _d3d_command_list->lpVtbl->Reset(_d3d_command_list, _command_allocator, _pipeline_state);
  if (FAILED(result)) {
    error(result, "_d3d_command_list Reset");
    exit(result);
  }
}
void _d3d_submit() {
  HRESULT result = _d3d_command_list->lpVtbl->Close(_d3d_command_list);
  if (FAILED(result)) {
    error(result, "_d3d_command_submit Close");
    exit(result);
  }
  _d3d_command_queue->lpVtbl->ExecuteCommandLists(
    _d3d_command_queue, 1, (ID3D12CommandList**)&_d3d_command_list
  );
  if (FAILED(result)) {
    error(result, "_d3d_command_submit Signal");
    exit(result);
  }
}

void _window_render() {
  DWORD task_index = 0;
  HANDLE mmtask = AvSetMmThreadCharacteristicsA("Games", &task_index);
  if (!mmtask) {
    error(GetLastError(), "window_run AvSetMmThreadCharacteristicsA");
  }
  f64 timer = time_ticks();
  do {
    u8 frame_index = _swapchain->lpVtbl->GetCurrentBackBufferIndex(_swapchain);
    // onresize
    if (window_resized) {
      window_resized = false;
      window_onresize();
    }
    const f64 now = time_ticks();
    window_deltatime = now - timer;
    timer = now;
#ifdef DEBUG
    if (window_deltatime > 1. / 60) {
      console_log("FPS DROP %f %f", 1. / 60, window_deltatime);
    }
#endif
    D3D12_RESOURCE_BARRIER barrier = {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
    };
    _vertices_length = 0;
    _indexes_length = 0;
    window_onrender();
    // D3D12_RESOURCE_STATE_COPY_DEST
    barrier.Transition.pResource = _d3d_buffer;
    barrier.Transition.StateBefore = D3D_BUFFER_TARGET_STATE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    // CopyBufferRegion
    u64 vertices_size = vertices_capacity * sizeof(vertex_t);
    _d3d_command_list->lpVtbl->CopyBufferRegion(
      _d3d_command_list,
      _d3d_buffer, 0,
      _d3d_buffer_upload, 0,
      vertices_size + indexes_capacity * sizeof(u32)
    );
    // D3D_BUFFER_TARGET_STATE
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D_BUFFER_TARGET_STATE;
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    // draw
    _d3d_command_list->lpVtbl->SetGraphicsRootSignature(_d3d_command_list, _d3d_root_signature);
    _d3d_command_list->lpVtbl->SetDescriptorHeaps(_d3d_command_list, 1, &_srv_heap);
    _d3d_command_list->lpVtbl->SetGraphicsRootDescriptorTable(_d3d_command_list, 0, _srv_gpu_handle);
    const D3D12_VIEWPORT viewport = {
      .Width = window_width,
      .Height = window_height,
      .MaxDepth = 1.f,
    };
    _d3d_command_list->lpVtbl->RSSetViewports(_d3d_command_list, 1, &viewport);
    const D3D12_RECT scissor_rect = { 0, 0, window_width, window_height };
    _d3d_command_list->lpVtbl->RSSetScissorRects(_d3d_command_list, 1, &scissor_rect);
    // D3D12_RESOURCE_STATE_RENDER_TARGET
    barrier.Transition.pResource = _d3d_render_targets[frame_index];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    const D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = {
      _rtv_handle.ptr + frame_index * _d3d_rtv_descriptor_size
    };
    _d3d_command_list->lpVtbl->OMSetRenderTargets(_d3d_command_list, 1, &rtv_handle, false, null);
    _d3d_command_list->lpVtbl->ClearRenderTargetView(_d3d_command_list, rtv_handle, (FLOAT*)&background_color, 0, 0);
    _d3d_command_list->lpVtbl->IASetPrimitiveTopology(_d3d_command_list, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const D3D12_VERTEX_BUFFER_VIEW vb_view = {
      .BufferLocation = _d3d_buffer->lpVtbl->GetGPUVirtualAddress(_d3d_buffer),
      .StrideInBytes = sizeof(vertex_t),
      .SizeInBytes = _vertices_length * sizeof(vertex_t),
    };
    _d3d_command_list->lpVtbl->IASetVertexBuffers(_d3d_command_list, 0, 1, &vb_view);
    const D3D12_INDEX_BUFFER_VIEW ib_view = {
      .BufferLocation = vb_view.BufferLocation + vertices_size,
      .Format = DXGI_FORMAT_R32_UINT,
      .SizeInBytes = _indexes_length * sizeof(u32),
    };
    _d3d_command_list->lpVtbl->IASetIndexBuffer(_d3d_command_list, &ib_view);
    _d3d_command_list->lpVtbl->DrawIndexedInstanced(_d3d_command_list, _indexes_length, 1, 0, 0, 0);
    // D3D12_RESOURCE_STATE_PRESENT
    barrier.Transition.pResource = _d3d_render_targets[frame_index];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    // present
    _d3d_submit();
    _swapchain->lpVtbl->Present(_swapchain, 1, 0);
    _d3d_wait();
    _d3d_reset();
  } while (_window_id);
  AvRevertMmThreadCharacteristics(mmtask);
  _d3d_wait();
  CloseHandle(_d3d_fence_event);
  for (u8 i = 0; i < D3D_FRAME_COUNT; i++) {
    _d3d_render_targets[i]->lpVtbl->Release(_d3d_render_targets[i]);
  }
  _d3d_texture->lpVtbl->Release(_d3d_texture);
  _d3d_fence->lpVtbl->Release(_d3d_fence);
  _pipeline_state->lpVtbl->Release(_pipeline_state);
  _command_allocator->lpVtbl->Release(_command_allocator);
  _d3d_command_queue->lpVtbl->Release(_d3d_command_queue);
  _d3d_command_list->lpVtbl->Release(_d3d_command_list);
  _srv_heap->lpVtbl->Release(_srv_heap);
  _rtv_heap->lpVtbl->Release(_rtv_heap);
  _d3d_root_signature->lpVtbl->Release(_d3d_root_signature);
  _d3d_buffer->lpVtbl->Release(_d3d_buffer);
  _d3d_buffer_upload->lpVtbl->Release(_d3d_buffer_upload);
  _swapchain->lpVtbl->Release(_swapchain);
  _d3d_device->lpVtbl->Release(_d3d_device);
}
void _gfx_startup(const char* atlas_path) {
  i32 result;
  u32 factory_flags = 0;
#ifdef DEBUG
  ID3D12Debug* debug_controller;
  D3D12GetDebugInterface(&IID_ID3D12Debug, (void**)&debug_controller);
  if (debug_controller) {
    debug_controller->lpVtbl->EnableDebugLayer(debug_controller);
    debug_controller->lpVtbl->Release(debug_controller);
    factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif
  IDXGIFactory6* factory;
  result = CreateDXGIFactory2(factory_flags, &IID_IDXGIFactory6, (void**)&factory);
  if (FAILED(result)) {
    error(result, "CreateDXGIFactory2");
    exit(result);
  }
  {
    // IDXGIAdapter
    IDXGIAdapter* adapter;
    result = factory->lpVtbl->EnumAdapters(factory, 0, &adapter);
    if (FAILED(result)) {
      result = factory->lpVtbl->EnumWarpAdapter(factory, &IID_IDXGIAdapter, (void**)&adapter);
      if (FAILED(result)) {
        error(result, "EnumWarpAdapter");
        exit(result);
      }
    }
    // ID3D12Device
    result = D3D12CreateDevice(
      (IUnknown*)adapter, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, (void**)&_d3d_device
    );
    adapter->lpVtbl->Release(adapter);
    if (FAILED(result)) {
      error(result, "CreateDXGIFactory2");
      exit(result);
    }
  }
  { // ID3D12CommandQueue
    const D3D12_COMMAND_QUEUE_DESC queue_desc = {
      .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
      .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
    };
    result = _d3d_device->lpVtbl->CreateCommandQueue(_d3d_device, &queue_desc, &IID_ID3D12CommandQueue, (void**)&_d3d_command_queue);
    if (FAILED(result)) {
      error(result, "CreateCommandQueue");
      exit(result);
    }
  }
  { // IDXGISwapChain3
    const DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
      .BufferCount = D3D_FRAME_COUNT,
      .Width = window_width,
      .Height = window_height,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
      .SampleDesc.Count = 1,
    };
    IDXGISwapChain1* swapchain;
    result = factory->lpVtbl->CreateSwapChainForHwnd(
      factory, (IUnknown*)_d3d_command_queue, _window_id, &swapchain_desc, null,
      null, &swapchain
    );
    factory->lpVtbl->Release(factory);
    if (FAILED(result)) {
      error(result, "CreateSwapChainForHwnd");
      exit(result);
    }
    result = swapchain->lpVtbl->QueryInterface(
      swapchain, &IID_IDXGISwapChain3, (void**)&_swapchain
    );
    swapchain->lpVtbl->Release(swapchain);
  }
  { // _rtv_heap
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
      .NumDescriptors = D3D_FRAME_COUNT,
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV
    };
    result = _d3d_device->lpVtbl->CreateDescriptorHeap(
      _d3d_device, &heap_desc, &IID_ID3D12DescriptorHeap, (void**)&_rtv_heap
    );
    if (FAILED(result)) {
      error(result, "_rtv_heap CreateDescriptorHeap");
      exit(result);
    }
    _rtv_heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(
      _rtv_heap, &_rtv_handle
    );
    _d3d_rtv_descriptor_size = _d3d_device->lpVtbl->GetDescriptorHandleIncrementSize(
      _d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV
    );
    // _srv_heap
    heap_desc.NumDescriptors = 1;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    result = _d3d_device->lpVtbl->CreateDescriptorHeap(
      _d3d_device, &heap_desc, &IID_ID3D12DescriptorHeap, (void**)&_srv_heap
    );
    if (result != S_OK) {
      error(result, "_srv_heap CreateDescriptorHeap");
      exit(result);
    }
    _srv_heap->lpVtbl->GetGPUDescriptorHandleForHeapStart(_srv_heap, &_srv_gpu_handle);
    if (_srv_gpu_handle.ptr == 0) {
      error(result, "_srv_heap GetGPUDescriptorHandleForHeapStart");
      exit(result);
    }
  }
  {
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = _rtv_handle;
    // Create a RTV for each frame.
    for (u8 i = 0; i < D3D_FRAME_COUNT; i++) {
      result = _swapchain->lpVtbl->GetBuffer(
        _swapchain, i, &IID_ID3D12Resource, (void**)&_d3d_render_targets[i]
      );
      _d3d_device->lpVtbl->CreateRenderTargetView(
        _d3d_device, _d3d_render_targets[i], null, rtv_handle
      );
      rtv_handle.ptr += _d3d_rtv_descriptor_size;
    }
  }
  result = _d3d_device->lpVtbl->CreateCommandAllocator(
    _d3d_device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator,
    (void**)&_command_allocator
  );
  if (FAILED(result)) {
    error(result, "CreateCommandAllocator");
    exit(result);
  }
  {
    const D3D12_DESCRIPTOR_RANGE srv_range = {
      .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
      .NumDescriptors = 1,
      .BaseShaderRegister = 0, // t0
      .RegisterSpace = 0,
      .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
    };
    const D3D12_ROOT_PARAMETER root_param = {
      .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
      .DescriptorTable = {
        .pDescriptorRanges = &srv_range,
        .NumDescriptorRanges = 1,
      },
      .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
    };
    const D3D12_STATIC_SAMPLER_DESC sampler = {
      .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
      .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
      .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
      .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
      .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
      .MaxLOD = D3D12_FLOAT32_MAX,
      .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
    };
    const D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {
      .NumParameters = 1,
      .pParameters = &root_param,
      .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
      .pStaticSamplers = &sampler,
      .NumStaticSamplers = 1,
    };
    ID3DBlob* signature;
    ID3DBlob* error_blob;
    result = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &error_blob);
    if (FAILED(result)) {
      error(result, "D3D12SerializeRootSignature");
      error_blob->lpVtbl->Release(error_blob);
    }
    _d3d_device->lpVtbl->CreateRootSignature(
      _d3d_device, 0, signature->lpVtbl->GetBufferPointer(signature),
      signature->lpVtbl->GetBufferSize(signature),
      &IID_ID3D12RootSignature, (void*)&_d3d_root_signature
    );
    signature->lpVtbl->Release(signature);
  }
  {
    const D3D12_INPUT_ELEMENT_DESC input_layout [] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    i32 vs_size;
    u8* vs_bytes = fs_readfile_sync("share/vs.cso", &vs_size);
    i32 ps_size;
    u8* ps_bytes = fs_readfile_sync("share/ps.cso", &ps_size);
    const D3D12_RENDER_TARGET_BLEND_DESC render_target_blend = {
      .BlendEnable = true,
      .SrcBlend = D3D12_BLEND_SRC_ALPHA,
      .DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
      .BlendOp = D3D12_BLEND_OP_ADD,
      .SrcBlendAlpha = D3D12_BLEND_ONE,
      .DestBlendAlpha = D3D12_BLEND_ZERO,
      .BlendOpAlpha = D3D12_BLEND_OP_ADD,
      .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
    };
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {
      .InputLayout = { input_layout, sizeof(input_layout) / sizeof(D3D12_INPUT_ELEMENT_DESC) },
      .pRootSignature = _d3d_root_signature,
      .VS = { vs_bytes, vs_size },
      .PS = { ps_bytes, ps_size },
      .RasterizerState = {
        .FillMode = D3D12_FILL_MODE_SOLID,
        .CullMode = D3D12_CULL_MODE_NONE,
        .DepthClipEnable = true,
      },
      .BlendState.RenderTarget[0] = render_target_blend,
      .SampleMask = UINT_MAX,
      .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
      .NumRenderTargets = 1,
      .RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc.Count = 1
    };
    result = _d3d_device->lpVtbl->CreateGraphicsPipelineState(
      _d3d_device, &pso_desc, &IID_ID3D12PipelineState, (void**)&_pipeline_state
    );
    if (FAILED(result)) {
      error(result, "CreateGraphicsPipelineState");
      exit(result);
    }
    memory_free(vs_bytes);
    memory_free(ps_bytes);
  }
  result = _d3d_device->lpVtbl->CreateCommandList(
    _d3d_device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, _command_allocator,
    _pipeline_state, &IID_ID3D12GraphicsCommandList, (void**)&_d3d_command_list
  );
  if (FAILED(result)) {
    error(result, "CreateCommandList");
    exit(result);
  }
  ID3D12Resource* texture_upload_heap;
  { // texture
    D3D12_RESOURCE_DESC resource_desc = {
      .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
      .Width = ATLAS_WIDTH,
      .Height = ATLAS_HEIGHT,
      .DepthOrArraySize = 1,
      .MipLevels = 1,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc.Count = 1,
    };
    D3D12_HEAP_PROPERTIES heap_props = {
      .Type = D3D12_HEAP_TYPE_DEFAULT
    };
    // texture_default
    result = _d3d_device->lpVtbl->CreateCommittedResource(
      _d3d_device,
      &heap_props,
      D3D12_HEAP_FLAG_NONE,
      &resource_desc,
      D3D12_RESOURCE_STATE_COPY_DEST,
      0,
      &IID_ID3D12Resource,
      (void**)&_d3d_texture
    );
    if (FAILED(result)) {
      error(result, "texture CreateCommittedResource");
      exit(result);
    }
    // footsprint
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    u32 num_rows;
    u64 row_size, total_size;
    _d3d_device->lpVtbl->GetCopyableFootprints(
      _d3d_device, &resource_desc, 0, 1, 0,
      &footprint, &num_rows, &row_size, &total_size
    );
    // texture_upload
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = total_size;
    resource_desc.Height = 1;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Format = 0;
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    result = _d3d_device->lpVtbl->CreateCommittedResource(
      _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
      D3D12_RESOURCE_STATE_GENERIC_READ, 0, &IID_ID3D12Resource,
      (void**)&texture_upload_heap
    );
    if (FAILED(result)) {
      error(result, "upload_texture CreateCommittedResource");
      exit(result);
    }
    // image_load
    u8* image = fs_readfilen_sync(atlas_path, ATLAS_WIDTH * ATLAS_HEIGHT * 4);
    if (image == null) {
      error(ERR_NOT_FOUND, "atlas_load");
      exit(result);
    }
    u8* dst_data;
    u8* src_data = image;
    texture_upload_heap->lpVtbl->Map(texture_upload_heap, 0, 0, (void**)&dst_data);
    for (u32 row = 0; row < num_rows; row++) {
      memory_copy(dst_data, src_data, row_size);
      dst_data += footprint.Footprint.RowPitch;
      src_data += row_size;
    }
    texture_upload_heap->lpVtbl->Unmap(texture_upload_heap, 0, null);
    memory_free(image);
    const D3D12_TEXTURE_COPY_LOCATION dst = {
      .pResource = _d3d_texture,
      .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
      .SubresourceIndex = 0,
    };
    const D3D12_TEXTURE_COPY_LOCATION src = {
      .pResource = texture_upload_heap,
      .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
      .PlacedFootprint = footprint,
    };
    _d3d_command_list->lpVtbl->CopyTextureRegion(_d3d_command_list, &dst, 0, 0, 0, &src, null);
    // D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    const D3D12_RESOURCE_BARRIER barrier = {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .pResource = _d3d_texture,
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
        .StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
      }
    };
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    // srv_handle
    D3D12_CPU_DESCRIPTOR_HANDLE srv_handle;
    _srv_heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_srv_heap, &srv_handle);
    // SRV
    const D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
      .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
      .Texture2D.MipLevels = 1,
    };
    _d3d_device->lpVtbl->CreateShaderResourceView(
      _d3d_device, _d3d_texture, &srv_desc, srv_handle
    );
  }
  _d3d_device->lpVtbl->CreateFence(
    _d3d_device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&_d3d_fence
  );
  _d3d_submit();
  _d3d_fence_event = CreateEventA(0, false, false, 0);
  _d3d_wait();
  _d3d_reset();
  texture_upload_heap->lpVtbl->Release(texture_upload_heap);
}
void vertices_reserve(u64 vertices_size, u64 indexes_size) {
  if (vertices_size == 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
    _d3d_buffer_upload->lpVtbl->Release(_d3d_buffer_upload);
    vertices_capacity = 0;
    return;
  }
  if (vertices_capacity != 0) {
    _d3d_buffer->lpVtbl->Release(_d3d_buffer);
    _d3d_buffer_upload->lpVtbl->Release(_d3d_buffer_upload);
  }
  const u64 vertices_buffer_size = vertices_size * sizeof(vertex_t);
  const u64 indexes_buffer_size = indexes_size * sizeof(u32);
  // d3d_buffers
  D3D12_HEAP_PROPERTIES heap_props = { .Type = D3D12_HEAP_TYPE_DEFAULT };
  const D3D12_RESOURCE_DESC resource_desc = {
    .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
    .Height = 1,
    .Width = vertices_buffer_size + indexes_buffer_size,
    .DepthOrArraySize = 1,
    .MipLevels = 1,
    .SampleDesc.Count = 1,
    .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
  };
  HRESULT result;
  // D3D12_RESOURCE_STATE_COMMON
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
    D3D12_RESOURCE_STATE_COMMON, NULL, &IID_ID3D12Resource, (void**)&_d3d_buffer
  );
  if (FAILED(result)) {
    error(result, "_d3d_buffer CreateCommittedResource");
    exit(result);
  }
  // D3D_BUFFER_TARGET_STATE
  const D3D12_RESOURCE_BARRIER barrier = {
    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
    .Transition = {
      .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
      .pResource = _d3d_buffer,
      .StateBefore = D3D12_RESOURCE_STATE_COMMON,
      .StateAfter = D3D_BUFFER_TARGET_STATE
    }
  };
  _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
  // D3D12_HEAP_TYPE_UPLOAD
  heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
    D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&_d3d_buffer_upload
  );
  if (FAILED(result)) {
    error(result, "_d3d_buffer_upload CreateCommittedResource");
    exit(result);
  }
  result = _d3d_buffer_upload->lpVtbl->Map(_d3d_buffer_upload, 0, 0, (void**)&_vertices_virtual);
  _indexes_virtual = (void*)_vertices_virtual + vertices_buffer_size;
  if (FAILED(result)) {
    error(result, "_d3d_vertices_upload Map");
    exit(result);
  }
  // update
  vertices_capacity = vertices_size;
  indexes_capacity = indexes_size;
  _d3d_submit();
  _d3d_wait();
  _d3d_reset();
}

#endif