#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#define D3D_FRAME_COUNT 2

ID3D12Device* _d3d_device;
IDXGISwapChain3* _swapchain;
ID3D12Resource* _d3d_render_targets[D3D_FRAME_COUNT];
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
D3D12_GPU_DESCRIPTOR_HANDLE _gpu_srv_handle;

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
}
void _d3d_command_submit() {
  HRESULT result = _d3d_command_list->lpVtbl->Close(_d3d_command_list);
  if (FAILED(result)) {
    error(result, "_d3d_command_submit Close");
    exit(result);
  }
  _d3d_command_queue->lpVtbl->ExecuteCommandLists(
    _d3d_command_queue, 1, (ID3D12CommandList**)&_d3d_command_list
  );
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
  u8 frame_index = _swapchain->lpVtbl->GetCurrentBackBufferIndex(_swapchain);
  console_log("render %d", frame_index);
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
    // result = factory->lpVtbl->EnumAdapterByGpuPreference(factory, 0, DXGI_GPU_PREFERENCE_MINIMUM_POWER, &IID_IDXGIAdapter, (void**)&adapter);
    // if (SUCCEEDED(result))
    //   goto create_device;
    result = factory->lpVtbl->EnumAdapters(factory, 0, &adapter);
    if (SUCCEEDED(result))
      goto create_device;
    if (FAILED(factory->lpVtbl->EnumWarpAdapter(factory, &IID_IDXGIAdapter, (void**)&adapter))) {
      error(result, "EnumWarpAdapter");
      exit(result);
    }
  create_device:
    // DXGI_ADAPTER_DESC adapter_desc;
    // adapter->lpVtbl->GetDesc(adapter, &adapter_desc);
    // console_log("adapter_desc %ls", adapter_desc.Description);
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
#ifdef DEBUG
  { // ID3D12InfoQueue
    ID3D12InfoQueue* info_queue;
    result = _d3d_device->lpVtbl->QueryInterface(_d3d_device, &IID_ID3D12InfoQueue, (void**)&info_queue);;
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
    const D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {
      .NumDescriptors = D3D_FRAME_COUNT,
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV
    };
    result = _d3d_device->lpVtbl->CreateDescriptorHeap(
      _d3d_device, &rtv_heap_desc, &IID_ID3D12DescriptorHeap, (void**)&_rtv_heap
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
  }
  { // _srv_heap
    const D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {
      .NumDescriptors = 1,
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
    };
    _d3d_device->lpVtbl->CreateDescriptorHeap(
      _d3d_device, &srv_heap_desc, &IID_ID3D12DescriptorHeap, (void**)_srv_heap
    );
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
    D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data;
  }
  factory->lpVtbl->Release(factory);
  _d3d_debug();
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  exit(0);
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
  D3D12_RESOURCE_DESC buffer_desc = {
    .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
    .Height = 1,
    .Width = vertices_buffer_size + indexes_buffer_size,
    .DepthOrArraySize = 1,
    .MipLevels = 1,
    .SampleDesc.Count = 1,
    .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
    .Format = DXGI_FORMAT_UNKNOWN
  };
  HRESULT result;
  // _d3d_vertices
  result = _d3d_device->lpVtbl->CreateCommittedResource(
    _d3d_device, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
    D3D12_RESOURCE_STATE_COMMON, NULL, &IID_ID3D12Resource, (void**)&_d3d_buffer
  );
  if (FAILED(result)) {
    error(result, "_d3d_vertices CreateCommittedResource");
    exit(result);
  }
  // ResourceBarrier
  const D3D12_RESOURCE_BARRIER barriers [] = {
    {
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = {
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .pResource = _d3d_buffer,
        .StateBefore = D3D12_RESOURCE_STATE_COMMON,
        .StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
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
    D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&_d3d_buffer_upload
  );
  if (FAILED(result)) {
    error(result, "_d3d_vertices_upload CreateCommittedResource");
    exit(result);
  }
  result = _d3d_buffer_upload->lpVtbl->Map(_d3d_buffer_upload, 0, 0, (void**)&_vertices_virtual);
  if (FAILED(result)) {
    error(result, "_d3d_vertices_upload Map");
    exit(result);
  }
  // update
  vertices_capacity = vertices_size;
  indexes_capacity = indexes_size;
  _d3d_command_submit();
  _d3d_gpu_wait();
}
void _gfx_destroy() {}

#endif