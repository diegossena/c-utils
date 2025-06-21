#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#define D3D_FRAME_COUNT 2

ID3D12Device* _d3d_device;
IDXGISwapChain3* _d3d_swapchain;
ID3D12DescriptorHeap* _d3d_rtv_heap;
ID3D12Resource* _d3d_render_targets[D3D_FRAME_COUNT];
ID3D12CommandQueue* _d3d_command_queue;
ID3D12CommandAllocator* _d3d_command_allocator;
ID3D12GraphicsCommandList* _d3d_command_list;
ID3D12PipelineState* _pipeline_state;
ID3D12RootSignature* _d3d_root_signature;
ID3D12Resource* _d3d_buffer;
ID3D12Resource* _d3d_indexes_upload;
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
  ID3D12InfoQueue* info_queue = NULL;
  if (SUCCEEDED(_d3d_device->lpVtbl->QueryInterface(_d3d_device, &IID_ID3D12InfoQueue, (void**)&info_queue))) {
    u64 count = info_queue->lpVtbl->GetNumStoredMessagesAllowedByRetrievalFilter(info_queue);
    for (u64 i = 0; i < count; i++) {
      u64 length = 0;
      info_queue->lpVtbl->GetMessageA(info_queue, i, null, &length);
      D3D12_MESSAGE* message = (D3D12_MESSAGE*)memory_alloc(length);
      if (message) {
        info_queue->lpVtbl->GetMessageA(info_queue, i, message, &length);
        console_log("[D3D12] %s", message->pDescription);
        memory_free(message);
      }
    }
    info_queue->lpVtbl->ClearStoredMessages(info_queue);
    info_queue->lpVtbl->Release(info_queue);
  }
}
void _d3d_gpu_wait() {
  HRESULT result;
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
  if (completed < _d3d_fence_signal) {
    _d3d_fence->lpVtbl->SetEventOnCompletion(_d3d_fence, _d3d_fence_signal, _d3d_fence_event);
    WaitForSingleObject(_d3d_fence_event, INFINITE);
  }
  if (_d3d_fence_value < _d3d_fence_signal) {
    _d3d_fence_value = _d3d_fence_signal;
    result = _d3d_command_allocator->lpVtbl->Reset(_d3d_command_allocator);
    if (FAILED(result)) {
      _d3d_debug();
      error(result, "_d3d_command_allocator Reset");
      exit(result);
    }
    result = _d3d_command_list->lpVtbl->Reset(_d3d_command_list, _d3d_command_allocator, _pipeline_state);
    if (FAILED(result)) {
      _d3d_debug();
      error(result, "_d3d_command_list Reset");
      exit(result);
    }
  }
  _d3d_frame_index = _d3d_swapchain->lpVtbl->GetCurrentBackBufferIndex(_d3d_swapchain);
}
void _d3d_command_submit() {
  HRESULT result = _d3d_command_list->lpVtbl->Close(_d3d_command_list);
  if (FAILED(result)) {
    error(result, "_d3d_command_submit Close");
    exit(result);
  }
  _d3d_command_queue->lpVtbl->ExecuteCommandLists(_d3d_command_queue, 1, (ID3D12CommandList**)&_d3d_command_list);
  result = _d3d_command_queue->lpVtbl->Signal(_d3d_command_queue, _d3d_fence, ++_d3d_fence_signal);
  if (FAILED(result)) {
    _d3d_debug();
    error(result, "_d3d_command_submit Signal");
    exit(result);
  }
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
    _window_resize();
    window_onresize();
  }
  _vertices_length = 0;
  _indexes_length = 0;
  _d3d_frame_index = _d3d_swapchain->lpVtbl->GetCurrentBackBufferIndex(_d3d_swapchain);
  console_log("render %d", _d3d_frame_index);
  window_onrender();
}
void _gfx_inicialize(const char* atlas_path) {
  i32 result;
  u32 factory_flags = 0;
#ifdef DEBUG
  ID3D12Debug* debug_controller;
  D3D12GetDebugInterface(&IID_ID3D12Debug, (void**)&debug_controller);
  if (debug_controller) {
    debug_controller->lpVtbl->EnableDebugLayer(debug_controller);
    debug_controller->lpVtbl->Release(debug_controller);
    factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
    // ID3D12InfoQueue
    ID3D12InfoQueue* info_queue;
    result = _d3d_device->lpVtbl->QueryInterface(_d3d_device, &IID_ID3D12InfoQueue, (void**)&info_queue);
    if (SUCCEEDED(result)) {
      info_queue->lpVtbl->SetBreakOnSeverity(info_queue, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
      info_queue->lpVtbl->SetBreakOnSeverity(info_queue, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
      info_queue->lpVtbl->SetBreakOnSeverity(info_queue, D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
      info_queue->lpVtbl->Release(info_queue);
    } else {
      error(result, "IID_ID3D12InfoQueue");
    }
  }
#endif
  IDXGIFactory4* factory;
  result = CreateDXGIFactory2(factory_flags, &IID_IDXGIFactory4, (void**)&factory);
  if (FAILED(result)) {
    error(result, "CreateDXGIFactory2");
    exit(result);
  }
  IDXGIAdapter* adapter;
  result = factory->lpVtbl->EnumWarpAdapter(factory, &IID_IDXGIAdapter, (void**)&adapter);
  result = D3D12CreateDevice(
    (IUnknown*)adapter, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, (void**)&_d3d_device
  );
  if (FAILED(result)) {
    error(result, "CreateDXGIFactory2");
    exit(result);
  }
  factory->lpVtbl->Release(factory);
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  exit(0);
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
    D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&_d3d_indexes_upload
  );
  if (FAILED(result)) {
    error(result, "_d3d_indexes_upload CreateCommittedResource");
    exit(result);
  }
  result = _d3d_indexes_upload->lpVtbl->Map(_d3d_indexes_upload, 0, 0, (void**)&_indexes_virtual);
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
  _d3d_swapchain->lpVtbl->Release(_d3d_swapchain);
  _d3d_rtv_heap->lpVtbl->Release(_d3d_rtv_heap);
  _d3d_command_queue->lpVtbl->Release(_d3d_command_queue);
  _d3d_command_allocator->lpVtbl->Release(_d3d_command_allocator);
  _d3d_command_list->lpVtbl->Release(_d3d_command_list);
  _pipeline_state->lpVtbl->Release(_pipeline_state);
  _d3d_root_signature->lpVtbl->Release(_d3d_root_signature);
  for (u8 i = 0; i < D3D_FRAME_COUNT; i++) {
    _d3d_render_targets[i]->lpVtbl->Release(_d3d_render_targets[i]);
  }
}

#endif