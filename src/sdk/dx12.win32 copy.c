#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#define D3D_FRAME_COUNT 2

ID3D12Device* _d3d_device;
IDXGISwapChain3* _swapchain;
ID3D12DescriptorHeap* _rtv_heap;
ID3D12Resource* _d3d_render_targets[D3D_FRAME_COUNT];
ID3D12CommandQueue* _d3d_command_queue;
ID3D12CommandAllocator* _command_allocator;
ID3D12GraphicsCommandList* _d3d_command_list;
ID3D12PipelineState* _pipeline_state;
ID3D12RootSignature* _d3d_root_signature;
ID3D12Resource* _d3d_vertices;
ID3D12Resource* _d3d_indexes;
ID3D12Resource* _d3d_vertices_upload;
ID3D12Resource* _d3d_buffer_upload;
D3D12_CPU_DESCRIPTOR_HANDLE _rtv_handle;
ID3D12DescriptorHeap* _srv_heap;
D3D12_GPU_DESCRIPTOR_HANDLE _gpu_srv_handle;
u8 _d3d_frame_index;
u8 _d3d_rtv_descriptor_size;

ID3D12Fence* _d3d_fence;
HANDLE _d3d_fence_event;
u64 _d3d_fence_value;
u64 _d3d_fence_signal;

void _window_resize() {}
void _d3d_debug() {
  ID3D12InfoQueue* infoQueue = NULL;
  if (SUCCEEDED(_d3d_device->lpVtbl->QueryInterface(_d3d_device, &IID_ID3D12InfoQueue, (void**)&infoQueue))) {
    infoQueue->lpVtbl->SetBreakOnSeverity(infoQueue, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    infoQueue->lpVtbl->SetBreakOnSeverity(infoQueue, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    infoQueue->lpVtbl->SetBreakOnSeverity(infoQueue, D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
    SIZE_T messageCount = infoQueue->lpVtbl->GetNumStoredMessagesAllowedByRetrievalFilter(infoQueue);
    console_log("messageCount %d", messageCount);
    for (SIZE_T i = 0; i < messageCount; i++) {
      SIZE_T messageLength = 0;
      infoQueue->lpVtbl->GetMessage(infoQueue, i, NULL, &messageLength); // Get size

      D3D12_MESSAGE* message = (D3D12_MESSAGE*)memory_alloc(messageLength);
      if (message) {
        infoQueue->lpVtbl->GetMessageA(infoQueue, i, message, &messageLength); // Get message content
        console_log("[D3D12] %s", message->pDescription);
        memory_free(message);
      }
    }

    infoQueue->lpVtbl->ClearStoredMessages(infoQueue);
    infoQueue->lpVtbl->Release(infoQueue);
  }
}
void _d3d_gpu_wait() {
  HRESULT result;
  u64 completed = _d3d_fence->lpVtbl->GetCompletedValue(_d3d_fence);
  // console_log("_d3d_gpu_wait _d3d_fence_value %d GetCompletedValue %d _d3d_fence_signal %d _d3d_fence_value %d",
  //   _d3d_fence_value,
  //   completed, _d3d_fence_signal, _d3d_fence_value
  // );
  if (completed == -1) {
    result = _d3d_device->lpVtbl->GetDeviceRemovedReason(_d3d_device);
    switch (result) {
      case DXGI_ERROR_DEVICE_HUNG:
        error(result, "_d3d_gpu_wait GetCompletedValue DXGI_ERROR_DEVICE_HUNG");
        break;
    }
    exit(result);
  } else if (completed != _d3d_fence_signal) {
    _d3d_fence->lpVtbl->SetEventOnCompletion(_d3d_fence, _d3d_fence_signal, _d3d_fence_event);
    WaitForSingleObject(_d3d_fence_event, INFINITE);
  }
  if (_d3d_fence_value != _d3d_fence_signal) {
    _d3d_fence_value = _d3d_fence_signal;
    result = _command_allocator->lpVtbl->Reset(_command_allocator);
    if (FAILED(result)) {
      _d3d_debug();
      error(result, "_d3d_command_allocator Reset");
      exit(result);
    }
    result = _d3d_command_list->lpVtbl->Reset(_d3d_command_list, _command_allocator, _pipeline_state);
    if (FAILED(result)) {
      _d3d_debug();
      error(result, "_d3d_command_list Reset");
      exit(result);
    }
  }
  // _d3d_frame_index = _d3d_swapchain->lpVtbl->GetCurrentBackBufferIndex(_d3d_swapchain);
  // _d3d_frame_index = (_d3d_frame_index + 1) % D3D_FRAME_COUNT;
  // console_log("!_d3d_gpu_wait _d3d_fence_value %d GetCompletedValue %d _d3d_frame_index %d _d3d_fence_value %d",
  //   _d3d_fence_value,
  //   _d3d_fence->lpVtbl->GetCompletedValue(_d3d_fence),
  //   _d3d_frame_index, _d3d_fence_value
  // );
}
void _d3d_command_submit() {
  HRESULT result = _d3d_command_list->lpVtbl->Close(_d3d_command_list);
  if (FAILED(result)) {
    error(result, "_d3d_command_submit Close");
    _d3d_debug();
    exit(result);
  }
  _d3d_command_queue->lpVtbl->ExecuteCommandLists(_d3d_command_queue, 1, (ID3D12CommandList**)&_d3d_command_list);
  _d3d_fence_signal = _d3d_fence_value + 1;
  result = _d3d_command_queue->lpVtbl->Signal(_d3d_command_queue, _d3d_fence, _d3d_fence_signal);
  if (FAILED(result)) {
    _d3d_debug();
    error(result, "_d3d_command_submit Signal");
    exit(result);
  }
}
void _vertices_free() {
  assert(indexes_capacity > 0);
  assert(vertices_capacity > 0);
  _d3d_vertices->lpVtbl->Release(_d3d_vertices);
  _d3d_indexes->lpVtbl->Release(_d3d_indexes);
  _d3d_vertices_upload->lpVtbl->Release(_d3d_vertices_upload);
  _d3d_buffer_upload->lpVtbl->Release(_d3d_buffer_upload);
}
void _window_render() {
  const f64 now = time_now_f64();
  const f64 delta_time = now - _render_time;
  window_deltatime = delta_time;
  _render_time = now;
#ifdef DEBUG
  const f64 frame_time = 1. / 60;
  if (window_deltatime > frame_time) {
    console_log("FPS DROP %f %f", frame_time, window_deltatime);
  }
#endif
  // onresize
  if (window_resized) {
    window_resized = false;
    _window_resize();
    window_onresize();
  }
  _vertices_length = 0;
  _indexes_length = 0;
  _d3d_frame_index = _swapchain->lpVtbl->GetCurrentBackBufferIndex(_swapchain);
  console_log("render %d", _d3d_frame_index);
  window_onrender();
  // (_d3d_vertices_data | _d3d_indexes_data) memory_copy
  assert(_vertices_length <= vertices_capacity);
  assert(_indexes_length <= indexes_capacity);
  const u64 vertices_size = _vertices_length * sizeof(vertex_t);
  const u64 indexes_size = _indexes_length * sizeof(u32);
  _d3d_command_list->lpVtbl->SetGraphicsRootSignature(_d3d_command_list, _d3d_root_signature);
  _d3d_command_list->lpVtbl->SetDescriptorHeaps(_d3d_command_list, 1, (ID3D12DescriptorHeap**)&_srv_heap);
  _d3d_command_list->lpVtbl->SetGraphicsRootDescriptorTable(_d3d_command_list, 0, _gpu_srv_handle);
  const D3D12_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = window_width,
    .Height = window_height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  _d3d_command_list->lpVtbl->RSSetViewports(_d3d_command_list, 1, &viewport);
  D3D12_RECT scissor_rect = { 0, 0, window_width, window_height };
  _d3d_command_list->lpVtbl->RSSetScissorRects(_d3d_command_list, 1, &scissor_rect);
  _d3d_command_list->lpVtbl->IASetPrimitiveTopology(_d3d_command_list, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  const D3D12_VERTEX_BUFFER_VIEW vb_view = {
    .BufferLocation = _d3d_vertices->lpVtbl->GetGPUVirtualAddress(_d3d_vertices),
    .StrideInBytes = sizeof(vertex_t),
    .SizeInBytes = vertices_size
  };
  _d3d_command_list->lpVtbl->IASetVertexBuffers(_d3d_command_list, 0, 1, &vb_view);
  const D3D12_INDEX_BUFFER_VIEW ib_view = {
    .BufferLocation = _d3d_indexes->lpVtbl->GetGPUVirtualAddress(_d3d_indexes),
    .Format = DXGI_FORMAT_R32_UINT,
    .SizeInBytes = indexes_size
  };
  _d3d_command_list->lpVtbl->IASetIndexBuffer(_d3d_command_list, &ib_view);
  // _d3d_command_list->lpVtbl->SetPipelineState(_d3d_command_list, _pipeline_state);
  // ResourceBarrier
  D3D12_RESOURCE_BARRIER barriers [] = {
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_vertices,
        .StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST,
      }
    },
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_indexes,
        .StateBefore = D3D12_RESOURCE_STATE_INDEX_BUFFER,
        .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST,
      }
    },
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_render_targets[_d3d_frame_index],
        .StateBefore = D3D12_RESOURCE_STATE_PRESENT,
        .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
      }
    },
  };
  _d3d_command_list->lpVtbl->ResourceBarrier(
    _d3d_command_list, 2,
    barriers
  );
  // CopyBufferRegion
  _d3d_command_list->lpVtbl->CopyBufferRegion(
    _d3d_command_list,
    _d3d_vertices, 0,
    _d3d_vertices_upload, 0,
    vertices_size
  );
  _d3d_command_list->lpVtbl->CopyBufferRegion(
    _d3d_command_list,
    _d3d_indexes, 0,
    _d3d_buffer_upload, 0,
    indexes_size
  );
  // D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER
  barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
  barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
  barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
  barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
  _d3d_command_list->lpVtbl->ResourceBarrier(
    _d3d_command_list, 2,
    barriers
  );
  // D3D12_RESOURCE_STATE_RENDER_TARGET
  _d3d_command_list->lpVtbl->ResourceBarrier(
    _d3d_command_list, 1,
    &barriers[2]
  );
  // OMSetRenderTargets
  const D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = {
    _rtv_handle.ptr + _d3d_frame_index * _d3d_rtv_descriptor_size
  };
  _d3d_command_list->lpVtbl->OMSetRenderTargets(_d3d_command_list, 1, &rtv_handle, false, null);
  // ClearRenderTargetView
  _d3d_command_list->lpVtbl->ClearRenderTargetView(_d3d_command_list, rtv_handle, (FLOAT*)&window_background, 0, 0);
  // DrawIndexedInstanced
  _d3d_command_list->lpVtbl->DrawIndexedInstanced(_d3d_command_list, _indexes_length, 1, 0, 0, 0);
  // D3D12_RESOURCE_STATE_PRESENT
  barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
  _d3d_command_list->lpVtbl->ResourceBarrier(
    _d3d_command_list, 1,
    &barriers[2]
  );
  // submit
  _d3d_command_submit();
  _swapchain->lpVtbl->Present(_swapchain, 0, 0);
  _d3d_gpu_wait();
}
void _gfx_inicialize(const char* atlas_path) {
#ifdef DEBUG
  ID3D12Debug* debug_controller;
  if (SUCCEEDED(D3D12GetDebugInterface(&IID_ID3D12Debug, (void**)&debug_controller))) {
    debug_controller->lpVtbl->EnableDebugLayer(debug_controller);
    debug_controller->lpVtbl->Release(debug_controller);
  }
#endif
  // IDXGIFactory1
  IDXGIFactory4* factory;
  i32 result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory4, (void**)&factory);
  if (FAILED(result)) {
    error(result, "CreateDXGIFactory");
  }
  // IDXGIAdapter
  IDXGIAdapter* adapter;
  result = factory->lpVtbl->EnumAdapters(factory, 0, &adapter);
  if (FAILED(result)) {
    error(result, "EnumAdapters");
  }
  // ID3D12Device
  result = D3D12CreateDevice((IUnknown*)adapter, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, (void**)&_d3d_device);
  if (FAILED(result)) {
    error(result, "D3D12CreateDevice");
  }
  adapter->lpVtbl->Release(adapter);
  // ID3D12CommandQueue
  const D3D12_COMMAND_QUEUE_DESC queue_desc = {
    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
  };
  result = _d3d_device->lpVtbl->CreateCommandQueue(_d3d_device, &queue_desc, &IID_ID3D12CommandQueue, (void**)&_d3d_command_queue);
  if (FAILED(result)) {
    error(result, "CreateCommandQueue");
  }
  // IDXGISwapChain
  DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
    .BufferCount = D3D_FRAME_COUNT,
    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .Width = window_width,
    .Height = window_height,
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
    .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    .SampleDesc.Count = 1,
    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH  // allow full-screen switching
  };
  IDXGISwapChain1* swapchain;
  result = factory->lpVtbl->CreateSwapChainForHwnd(
    factory, (IUnknown*)_d3d_command_queue, _window_id, &swapchain_desc, 0, 0,
    &swapchain
  );
  if (FAILED(result)) {
    error(result, "CreateSwapChain");
    exit(result);
  }
  factory->lpVtbl->Release(factory);
  result = swapchain->lpVtbl->QueryInterface(swapchain, &IID_IDXGISwapChain3, (void**)&_swapchain);
  swapchain->lpVtbl->Release(swapchain);
  if (FAILED(result)) {
    error(result, "QueryInterface IID_IDXGISwapChain3");
  }
  // RTV
  const D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {
    .NumDescriptors = D3D_FRAME_COUNT,
    .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV
  };
  _d3d_device->lpVtbl->CreateDescriptorHeap(_d3d_device, &rtv_heap_desc, &IID_ID3D12DescriptorHeap, (void**)&_rtv_heap);
  _d3d_rtv_descriptor_size = _d3d_device->lpVtbl->GetDescriptorHandleIncrementSize(_d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  _rtv_heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_rtv_heap, &_rtv_handle);
  D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = _rtv_handle;
  for (u8 i = 0; i < D3D_FRAME_COUNT; i++) {
    swapchain->lpVtbl->GetBuffer(swapchain, i, &IID_ID3D12Resource, (void**)&_d3d_render_targets[i]);
    _d3d_device->lpVtbl->CreateRenderTargetView(
      _d3d_device, _d3d_render_targets[i], 0, rtv_handle
    );
    rtv_handle.ptr += _d3d_rtv_descriptor_size;
  }
  // SRV
  const D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {
    .NumDescriptors = 1,
    .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
    .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
  };
  _d3d_device->lpVtbl->CreateDescriptorHeap(
    _d3d_device, &srv_heap_desc, &IID_ID3D12DescriptorHeap, (void**)&_srv_heap
  );
  _srv_heap->lpVtbl->GetGPUDescriptorHandleForHeapStart(_srv_heap, &_gpu_srv_handle);
  // ID3D12RootSignature
  const D3D12_STATIC_SAMPLER_DESC sampler_desc = {
    .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
    .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
    .MinLOD = 0,
    .MaxLOD = D3D12_FLOAT32_MAX,
    .ShaderRegister = 0,
    .RegisterSpace = 0,
    .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
  };
  const D3D12_DESCRIPTOR_RANGE srv_range = {
    .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
    .NumDescriptors = 1,
    .BaseShaderRegister = 0, // t0
    .RegisterSpace = 0,
    .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
    // .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
  };
  const D3D12_ROOT_PARAMETER root_param = {
    .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
    .DescriptorTable = {
      .NumDescriptorRanges = 1,
      .pDescriptorRanges = &srv_range
    },
    .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
  };
  const D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {
    .NumParameters = 1,
    .pParameters = &root_param,
    .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
    .pStaticSamplers = &sampler_desc,
    .NumStaticSamplers = 1,
  };
  ID3DBlob* signature_blob;
  result = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, 0);
  if (FAILED(result)) {
    error(result, "D3D12SerializeRootSignature");
  }
  _d3d_device->lpVtbl->CreateRootSignature(
    _d3d_device, 0, signature_blob->lpVtbl->GetBufferPointer(signature_blob),
    signature_blob->lpVtbl->GetBufferSize(signature_blob),
    &IID_ID3D12RootSignature, (void*)&_d3d_root_signature
  );
  signature_blob->lpVtbl->Release(signature_blob);
  // CreateGraphicsPipelineState
  const D3D12_INPUT_ELEMENT_DESC input_layout [] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,               0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(f32) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
  };
  i32 vs_size;
  u8* vs_bytes = fs_readfile_sync("share/vs.cso", &vs_size);
  i32 ps_size;
  u8* ps_bytes = fs_readfile_sync("share/ps.cso", &ps_size);
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
    .BlendState.RenderTarget[0] = {
      .BlendEnable = false,
      .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
    },
    .SampleMask = UINT_MAX,
    .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    .NumRenderTargets = 1,
    .RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM,
    .SampleDesc.Count = 1
  };
  result = _d3d_device->lpVtbl->CreateGraphicsPipelineState(_d3d_device, &pso_desc, &IID_ID3D12PipelineState, (void**)&_pipeline_state);
  if (FAILED(result)) {
    _d3d_debug();
    error(result, "CreateGraphicsPipelineState");
    exit(result);
  }
  memory_free(vs_bytes);
  memory_free(ps_bytes);
  // ID3D12CommandAllocator
  _d3d_device->lpVtbl->CreateCommandAllocator(_d3d_device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void**)&_command_allocator);
  // ID3D12GraphicsCommandList 
  _d3d_device->lpVtbl->CreateCommandList(_d3d_device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, _command_allocator, _pipeline_state, &IID_ID3D12GraphicsCommandList, (void**)&_d3d_command_list);
  // CreateFence
  _d3d_device->lpVtbl->CreateFence(
    _d3d_device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&_d3d_fence
  );
  _d3d_fence_event = CreateEventA(0, false, false, 0);
  // TextureCreate
  ID3D12Resource* texture;
  ID3D12Resource* upload_heap;
  if (atlas_path) {
    const u64 atlas_size = atlas_width * atlas_height * 4;
    // Texture
    const D3D12_RESOURCE_DESC tex_desc = {
      .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
      .Width = atlas_width,
      .Height = atlas_height,
      .DepthOrArraySize = 1,
      .MipLevels = 1,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc = {.Count = 1, .Quality = 0 },
      .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
      .Flags = D3D12_RESOURCE_FLAG_NONE,
    };
    D3D12_HEAP_PROPERTIES heap_props = {
      .Type = D3D12_HEAP_TYPE_DEFAULT
    };
    result = _d3d_device->lpVtbl->CreateCommittedResource(
      _d3d_device,
      &heap_props,
      D3D12_HEAP_FLAG_NONE,
      &tex_desc,
      D3D12_RESOURCE_STATE_COPY_DEST,
      0,
      &IID_ID3D12Resource,
      (void**)&texture
    );
    if (FAILED(result)) {
      error(result, "texture CreateCommittedResource");
      exit(result);
    }
    // upload_heap
    const D3D12_RESOURCE_DESC upload_desc = {
      .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
      .Width = atlas_size,
      .Height = 1,
      .DepthOrArraySize = 1,
      .MipLevels = 1,
      .SampleDesc.Count = 1,
      .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR
    };
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    result = _d3d_device->lpVtbl->CreateCommittedResource(
      _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &upload_desc,
      D3D12_RESOURCE_STATE_GENERIC_READ, 0, &IID_ID3D12Resource,
      (void**)&upload_heap
    );
    if (FAILED(result)) {
      error(result, "upload_texture CreateCommittedResource");
      exit(result);
    }
    // texture = upload_heap
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    u32 num_rows;
    u64 row_size, total_size;
    _d3d_device->lpVtbl->GetCopyableFootprints(
      _d3d_device, &tex_desc, 0, 1, 0,
      &footprint, &num_rows, &row_size, &total_size
    );
    const D3D12_TEXTURE_COPY_LOCATION dst = {
      .pResource = texture,
      .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
      .SubresourceIndex = 0,
    };
    const D3D12_TEXTURE_COPY_LOCATION src = {
      .pResource = upload_heap,
      .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
      .PlacedFootprint = footprint,
    };
    // upload_heap copy
    u8* atlas_bytes = fs_readfilen_sync(atlas_path, atlas_size);
    if (atlas_bytes == null) {
      error(ERR_NOT_FOUND, "upload_heap copy");
    }
    void* upload_data;
    upload_heap->lpVtbl->Map(upload_heap, 0, 0, (void*)&upload_data);
    for (u32 y = 0; y < atlas_height; ++y) {
      memcpy(
        (u8*)upload_data + y * footprint.Footprint.RowPitch,
        atlas_bytes + y * atlas_width * 4,
        atlas_width * 4
      );
    }
    memory_copy(upload_data, atlas_bytes, atlas_size);
    upload_heap->lpVtbl->Unmap(upload_heap, 0, null);
    memory_free(atlas_bytes);
    _d3d_command_list->lpVtbl->CopyTextureRegion(_d3d_command_list, &dst, 0, 0, 0, &src, null);
    // D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    const D3D12_RESOURCE_BARRIER barrier = {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .pResource = texture,
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
        .StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
      }
    };
    _d3d_command_list->lpVtbl->ResourceBarrier(_d3d_command_list, 1, &barrier);
    // srv_handle
    D3D12_CPU_DESCRIPTOR_HANDLE srv_handle;
    _srv_heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_srv_heap, &srv_handle);
    // ShaderResourceView
    const D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
      .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
      .Format = tex_desc.Format,
      .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
      .Texture2D.MipLevels = 1
    };
    _d3d_device->lpVtbl->CreateShaderResourceView(
      _d3d_device, texture, &srv_desc, srv_handle
    );
  }
  _d3d_command_submit();
  _d3d_gpu_wait();
  texture->lpVtbl->Release(texture);
  upload_heap->lpVtbl->Release(upload_heap);
}
void vertices_reserve(u64 vertices_size, u64 indexes_size) {
  if (vertices_size == 0) {
    _vertices_free();
    vertices_capacity = 0;
    return;
  }
  if (vertices_capacity != 0) {
    _vertices_free();
  }
  const u64 vertices_buffer_size = vertices_size * sizeof(vertex_t);
  const u64 indexes_buffer_size = indexes_size * sizeof(u32);
  // d3d_buffers
  D3D12_HEAP_PROPERTIES heap_props = { .Type = D3D12_HEAP_TYPE_DEFAULT };
  D3D12_RESOURCE_DESC buffer_desc = {
    .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
    .Height = 1,
    .DepthOrArraySize = 1,
    .MipLevels = 1,
    .SampleDesc.Count = 1,
    .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
    .Format = DXGI_FORMAT_UNKNOWN
  };
  HRESULT result;
  // _d3d_vertices
  buffer_desc.Width = vertices_buffer_size;
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
    D3D12_RESOURCE_STATE_COMMON, NULL, &IID_ID3D12Resource, (void**)&_d3d_vertices
  );
  if (FAILED(result)) {
    error(result, "_d3d_vertices CreateCommittedResource");
    exit(result);
  }
  // indexes_buffer
  buffer_desc.Width = indexes_buffer_size;
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
    D3D12_RESOURCE_STATE_COMMON, NULL, &IID_ID3D12Resource,
    (void**)&_d3d_indexes
  );
  if (FAILED(result)) {
    error(result, "_d3d_indexes CreateCommittedResource");
    exit(result);
  }
  // ResourceBarrier
  const D3D12_RESOURCE_BARRIER barriers [] = {
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_vertices,
        .StateBefore = D3D12_RESOURCE_STATE_COMMON,
        .StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
      }
    },
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_indexes,
        .StateBefore = D3D12_RESOURCE_STATE_COMMON,
        .StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER
      }
    },
  };
  _d3d_command_list->lpVtbl->ResourceBarrier(
    _d3d_command_list, sizeof(barriers) / sizeof(D3D12_RESOURCE_BARRIER),
    barriers
  );
  // D3D12_HEAP_TYPE_UPLOAD
  heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
  // _d3d_vertices_upload
  buffer_desc.Width = vertices_buffer_size;
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
    D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&_d3d_vertices_upload
  );
  if (FAILED(result)) {
    error(result, "_d3d_vertices_upload CreateCommittedResource");
    exit(result);
  }
  result = _d3d_vertices_upload->lpVtbl->Map(_d3d_vertices_upload, 0, 0, (void**)&_vertices_virtual);
  if (FAILED(result)) {
    error(result, "_d3d_vertices_upload Map");
    exit(result);
  }
  // _d3d_indexes_upload
  buffer_desc.Width = indexes_buffer_size;
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
    D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&_d3d_buffer_upload
  );
  if (FAILED(result)) {
    error(result, "_d3d_indexes_upload CreateCommittedResource");
    exit(result);
  }
  result = _d3d_buffer_upload->lpVtbl->Map(_d3d_buffer_upload, 0, 0, (void**)&_indexes_virtual);
  if (FAILED(result)) {
    error(result, "_d3d_indexes_upload Map");
    exit(result);
  }
  // update
  vertices_capacity = vertices_size;
  indexes_capacity = indexes_size;
  _d3d_command_submit();
  _d3d_gpu_wait();
}
void _gfx_destroy() {
  console_log("_gfx_destroy");
  _d3d_gpu_wait();
  _d3d_device->lpVtbl->Release(_d3d_device);
  _swapchain->lpVtbl->Release(_swapchain);
  _rtv_heap->lpVtbl->Release(_rtv_heap);
  _d3d_command_queue->lpVtbl->Release(_d3d_command_queue);
  _command_allocator->lpVtbl->Release(_command_allocator);
  _d3d_command_list->lpVtbl->Release(_d3d_command_list);
  _pipeline_state->lpVtbl->Release(_pipeline_state);
  _d3d_root_signature->lpVtbl->Release(_d3d_root_signature);
  for (u8 i = 0; i < D3D_FRAME_COUNT; i++) {
    _d3d_render_targets[i]->lpVtbl->Release(_d3d_render_targets[i]);
  }
}

#endif