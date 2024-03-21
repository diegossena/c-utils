#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window.h>
#include <sdk/error.h>

#define wpath_dirname(this, length) { \
  wchar_t* __ptr = this + length - 1; \
  while (--length && *__ptr != L'\\' && *__ptr != L'/') { \
    --__ptr; \
  } \
  *__ptr = L'\0'; \
}
#define wpath_join(this, length, src, src_length) { \
  wchar_t* __ptr = this + length - 1; \
  if (*__ptr != L'\\' && *__ptr != L'/') { \
    *++__ptr =  L'\\'; \
    ++length; \
  } \
  __ptr = this + length; \
  memory_copy(__ptr, src, sizeof(wchar_t) * src_length + 2); \
  __ptr += length; \
  *__ptr = L'\0'; \
  length += length; \
}
#define wpath_join_cwstr(this, length, cstrw) { \
  u64 __cstrw_length = sizeof(cstrw) / 2 - 2; \
  wpath_join(this, length, cstrw, __cstrw_length) \
}

typedef struct window_t {
  void* context;
  // window
  HWND __hwnd;
  bool __mouse_tracking;
  // fonts
  queue_t __fonts; // __font_queue_t
  IDWriteFontCollection* __collection;
  // events
  update_event_cb onupdate;
  mouse_event_cb onmousemove;
  mouse_event_cb onmousedown;
  mouse_event_cb onmouseup;
  mouse_event_cb ondblclick;
  keyboard_event_cb onkeydown;
  keyboard_event_cb onkeyup;
  ui_event_cb onresize;
  window_event_cb onload;
  window_event_cb oncreate;
  window_event_cb onclose;
  // 3d_renderer
  IDXGISwapChain* __swapchain;
  ID3D11Device* __device;
  ID3D11DeviceContext* __device_context;
  ID3D11RenderTargetView* __backbuffer;
  ID3D11RasterizerState* __rasterizer_state;
  ID3D11VertexShader* __vertex_shader;
  ID3D11InputLayout* __input_layout;
  ID3D11PixelShader* __pixel_shader;
  // 2d_renderer
  ID2D1Factory* __d2d_factory;
  IDXGISurface* __d2d_surface;
  ID2D1RenderTarget* __d2d_render_target;
  IDWriteFactory* __d2d_write_factory;
} window_t;

#include <sdk/window/gfx/directdraw/FontCollectionLoader.win32.h>

u16 window_get_width(window_t* this) {
  RECT rect;
  GetWindowRect(this->__hwnd, &rect);
  return rect.right - rect.left;
}
u16 window_get_height(window_t* this) {
  RECT rect;
  GetWindowRect(this->__hwnd, &rect);
  return rect.bottom - rect.top;
}
void window_set_size(window_t* this, u32 width, u32 height) {
  RECT rect = { 0, 0, width, height };
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
  SetWindowPos(
    this->__hwnd, null,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME
  );
}
vector2d_t window_get_cursor(window_t* this) {
  POINT point;
  GetCursorPos(&point);
  ScreenToClient(this->__hwnd, &point);
  return (vector2d_t) { point.x, point.y };
}
void window_set_viewport(window_t* this, u32 width, u32 height) {
  if (this->__backbuffer) {
    ID2D1RenderTarget_Release(this->__d2d_render_target);
    IDXGISurface_Release(this->__d2d_surface);
    ID3D11RenderTargetView_Release(this->__backbuffer);
  }
  D3D11_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = (f32)width,
    .Height = (f32)height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  ID3D11DeviceContext_RSSetViewports(this->__device_context, 1, &viewport);
  IDXGISwapChain_ResizeBuffers(
    this->__swapchain, 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0
  );
  // get backbuffer
  ID3D11Texture2D* backbuffer;
  IDXGISwapChain_GetBuffer(this->__swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
  /**
   * use the back buffer address to create the render target
   * set the render target as the back buffer
   */
  ID3D11Device_CreateRenderTargetView(
    this->__device, (ID3D11Resource*)backbuffer, null, &this->__backbuffer
  );
  ID3D11DeviceContext_OMSetRenderTargets(
    this->__device_context, 1, &this->__backbuffer, 0
  );
  // set d2_render_target
  ID3D11Texture2D_QueryInterface(
    backbuffer, &IID_IDXGISurface, (void**)&this->__d2d_surface
  );
  D2D1_RENDER_TARGET_PROPERTIES renter_target_props = {
    .type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
    .pixelFormat = {
      .format = DXGI_FORMAT_UNKNOWN,
      .alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED
    }
  };
  HRESULT result = ID2D1Factory_CreateDxgiSurfaceRenderTarget(
    this->__d2d_factory, this->__d2d_surface, &renter_target_props,
    &this->__d2d_render_target
  );
  if (FAILED(result)) {
    error("CreateDxgiSurfaceRenderTarget", result);
  }
  // free resources
  ID3D11Texture2D_Release(backbuffer);
}
void window_free(window_t* this) {
  if (this->onload) {
    IDWriteFontCollection_Release(this->__collection);
  }
  // D2D
  ID2D1RenderTarget_Release(this->__d2d_render_target);
  IDXGISurface_Release(this->__d2d_surface);
  ID2D1Factory_Release(this->__d2d_factory);
  // D3D
  if (this->__input_layout) {
    ID3D11PixelShader_Release(this->__pixel_shader);
    ID3D11InputLayout_Release(this->__input_layout);
    ID3D11VertexShader_Release(this->__vertex_shader);
  }
  ID3D11RasterizerState_Release(this->__rasterizer_state);
  ID3D11RenderTargetView_Release(this->__backbuffer);
  ID3D11DeviceContext_Release(this->__device_context);
  ID3D11Device_Release(this->__device);
  IDXGISwapChain_Release(this->__swapchain);
  // window_t
  this->__hwnd = 0;
  memory_free(this);
}
void __window_mouse_tracking(window_t* this) {
  if (!this->__mouse_tracking && this->onmousemove) {
    this->__mouse_tracking = true;
    TRACKMOUSEEVENT tme = {
      .dwHoverTime = 800,
      .cbSize = sizeof(TRACKMOUSEEVENT),
      .dwFlags = TME_LEAVE,
      .hwndTrack = this->__hwnd
    };
    TrackMouseEvent(&tme);
  }
}
vector2d_t __window_mouse_event_cursor_handler(LPARAM lParam) {
  return (vector2d_t) { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
}
void __window_update_handler(HWND handle, UINT unused1, UINT_PTR unused2, DWORD unused3) {
  console_log("WM_TIMER");
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  const FLOAT background_color [] = { 1.0f, 1.0f, 1.0f, 1.0f };
  ID3D11DeviceContext_ClearRenderTargetView(
    this->__device_context, this->__backbuffer, background_color
  );
  ID2D1RenderTarget_BeginDraw(this->__d2d_render_target);
  if (this->onupdate) {
    this->onupdate(this);
  }
  ID2D1RenderTarget_EndDraw(this->__d2d_render_target, null, null);
  IDXGISwapChain_Present(this->__swapchain, 1, 0);
}
LRESULT __window_event_handler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_MOUSELEAVE:
      if (this->onmousemove) {
        this->__mouse_tracking = false;
        this->onmousemove(this, (vector2d_t) { -1, -1 });
      }
      return 0;
    case WM_MOUSEMOVE:
      if (this->onmousemove) {
        __window_mouse_tracking(this);
        this->onmousemove(this, __window_mouse_event_cursor_handler(lParam));
      }
      return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
      if (this->onmousedown) {
        this->onmousedown(this, __window_mouse_event_cursor_handler(lParam));
      }
      return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
      if (this->onmouseup) {
        this->onmouseup(this, __window_mouse_event_cursor_handler(lParam));
      }
      return 0;
    case WM_LBUTTONDBLCLK:
      if (this->ondblclick) {
        this->ondblclick(this, __window_mouse_event_cursor_handler(lParam));
      }
      return 0;
    case WM_KEYDOWN:
      if (this->onkeydown) {
        this->onkeydown(this);
      }
      return 0;
    case WM_KEYUP:
      if (this->onkeyup) {
        this->onkeyup(this);
      }
      return 0;
    case WM_SIZE:
      if (this->onresize) {
        this->onresize(this);
      }
      return 0;
    case WM_CREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      this = (window_t*)pcs->lpCreateParams;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)this);
      return 0;
    };
    case WM_CLOSE:
      if (this->onclose) {
        this->onclose(this);
      }
      KillTimer(handle, 0);
      DestroyWindow(handle);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      window_free(this);
      return 0;
  }
  return DefWindowProc(handle, message, wParam, lParam);
}

void window_startup(application_t* app, window_options_t* options) {
  LPCSTR class_name = "MainWClass";
  if (FindWindowA(class_name, null)) {
    error("FindWindowA", ERR_UNKNOWN);
    return;
  }
  window_t* this = memory_alloc0(sizeof(window_t));
  if (!this) {
    error("alloc::window_t", ERR_NOT_ENOUGH_MEMORY);
    return;
  }
  // RegisterWindowClass
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = __window_event_handler,
    .hInstance = GetModuleHandleA(null),
    .hCursor = LoadCursor(null, IDC_ARROW),
    .lpszClassName = class_name,
  };
  if (!RegisterClassExA(&wc)) {
    error("RegisterClassExA", ERR_UNKNOWN);
    goto clear;
  }
  u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
  if (!(options->flags & WINDOW_HIDDEN)) {
    window_style |= WS_VISIBLE;
  }
  if (!(options->flags & WINDOW_NO_RESIZABLE)) {
    window_style |= WS_THICKFRAME;
  }
  if (!(options->flags & WINDOW_NO_MAXIMIZE)) {
    window_style |= WS_MAXIMIZEBOX;
  }
  u32 window_ex_style = WS_EX_APPWINDOW;
  // Register Events
  this->onupdate = options->onupdate;
  this->onmousemove = options->onmousemove;
  this->onmousedown = options->onmousedown;
  this->onmouseup = options->onmouseup;
  this->onkeydown = options->onkeydown;
  this->onkeyup = options->onkeyup;
  this->ondblclick = options->ondblclick;
  this->onresize = options->onresize;
  this->onload = options->onload;
  this->oncreate = options->oncreate;
  this->onclose = options->onclose;
  // CreateWindow
  RECT rect = { options->x, options->y, options->width, options->height };
  AdjustWindowRect(&rect, window_style, false);
  u32 width = rect.right - rect.left;
  u32 height = rect.bottom - rect.top;
  this->__hwnd = CreateWindowExA(
    window_ex_style, wc.lpszClassName, options->name, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->__hwnd) {
    error("CreateWindowExA", ERR_UNKNOWN);
    goto clear;
  }
  // renderer
  HRESULT result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, null,
    (void**)&this->__d2d_factory
  );
  if (FAILED(result)) {
    error("D2D1CreateFactory", result);
  }
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&this->__d2d_write_factory
  );
  if (FAILED(result)) {
    error("DWriteCreateFactory", result);
  }
  DXGI_SWAP_CHAIN_DESC scd = { };
  scd.BufferCount = 1;                                // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
  scd.BufferDesc.Width = width;                     // set the back buffer width
  scd.BufferDesc.Height = height;                    // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
  scd.OutputWindow = this->__hwnd;                    // the window to be used
  scd.SampleDesc.Count = 4;                           // how many multisamples
  scd.Windowed = TRUE;                                // windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
  result = D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0,
    // Required for Direct2D interoperability with Direct3D resources.
    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    0,
    0,
    D3D11_SDK_VERSION,
    &scd,
    &this->__swapchain,
    &this->__device,
    0,
    &this->__device_context
  );
  if (FAILED(result)) {
    console_warn("D3D11CreateDeviceAndSwapChain %x", result);
    exit(1);
  }
  window_set_viewport(this, width, height);
  D3D11_RASTERIZER_DESC rasterizer_desc = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE
  };
  ID3D11Device_CreateRasterizerState(this->__device, &rasterizer_desc, &this->__rasterizer_state);
  if (this->onload) {
    queue_head(&this->__fonts);
    this->onload(this);
    // fonts
    SDK_FontCollectionLoader collection_loader;
    FontCollectionLoader_Inicialize(&collection_loader, &this->__fonts);
    this->__d2d_write_factory->lpVtbl->RegisterFontCollectionLoader(
      this->__d2d_write_factory, (IDWriteFontCollectionLoader*)&collection_loader
    );
    this->__d2d_write_factory->lpVtbl->CreateCustomFontCollection(
      this->__d2d_write_factory, (IDWriteFontCollectionLoader*)&collection_loader,
      0, 0, &this->__collection
    );
    this->__d2d_write_factory->lpVtbl->UnregisterFontCollectionLoader(
      this->__d2d_write_factory, (IDWriteFontCollectionLoader*)&collection_loader
    );
    queue_foreach(__font_queue_t, this->__fonts, it, it->queue.next) {
      memory_free(it);
      queue_remove((queue_t*)it);
    }
  }
  // events
  if (this->oncreate) {
    this->oncreate(this);
  }
  __window_mouse_tracking(this);
  SetTimer(this->__hwnd, 0, 1000 / 60, __window_update_handler);
  return;
clear:
  memory_free(this);
}

bool window_pooling() {
  MSG msg;
  WINBOOL result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
  if (result) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      return false;
    }
  }
  return true;
}

// void shader_load_fill(window_t* window) {
//   if (window->input_layout) {
//     ID3D11InputLayout_Release(window->input_layout);
//     ID3D11VertexShader_Release(window->vertex_shader);
//     ID3D11PixelShader_Release(window->pixel_shader);
//   }
//   ID3DBlob* blob;
//   HRESULT result;
//   wchar_t path[MAX_PATH] = {};
//   u64 path_length = GetModuleFileNameW(NULL, path, MAX_PATH);
//   wpath_dirname(path, path_length);
//   // layout
//   D3D11_INPUT_ELEMENT_DESC ied [] = {
//     {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//     {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
//   };
//   // vertex_shader
//   wpath_join_cwstr(path, path_length, L"vs_fill.cso");
//   result = D3DReadFileToBlob(path, &blob);
//   if (result != 0) {
//     error("D3DReadFileToBlob::vs_fill.cso", result);
//     return;
//   }
//   result = ID3D11Device_CreateVertexShader(
//     window->device, ID3D10Blob_GetBufferPointer(blob),
//     ID3D10Blob_GetBufferSize(blob), 0, &window->vertex_shader
//   );
//   if (result != S_OK) {
//     error("CreateVertexShader::fill", result);
//     goto vertex_shader_free;
//   }
//   // input_layout
//   result = ID3D11Device_CreateInputLayout(
//     window->device, ied, 2, ID3D10Blob_GetBufferPointer(blob),
//     ID3D10Blob_GetBufferSize(blob), &window->input_layout
//   );
//   if (result != S_OK) {
//     error("CreateInputLayout", result);
//     goto input_layout_free;
//   }
//   wpath_dirname(path, path_length);
//   wpath_join_cwstr(path, path_length, L"ps_fill.cso");
//   ID3D10Blob_Release(blob);
//   result = D3DReadFileToBlob(path, &blob);
//   if (result != 0) {
//     error("D3DReadFileToBlob::ps_fill.cso", result);
//     goto pixel_shader_free;
//   }
//   result = ID3D11Device_CreatePixelShader(
//     window->device, ID3D10Blob_GetBufferPointer(blob),
//     ID3D10Blob_GetBufferSize(blob), 0, &window->pixel_shader
//   );
//   if (result != S_OK) {
//     error("ID3D11Device_CreatePixelShader::fill", result);
//     goto pixel_shader_free;
//   }
//   return;
// pixel_shader_free:
//   ID3D11PixelShader_Release(window->pixel_shader);
//   window->pixel_shader = 0;
// input_layout_free:
//   ID3D11InputLayout_Release(window->input_layout);
//   window->input_layout = 0;
// vertex_shader_free:
//   ID3D11VertexShader_Release(window->vertex_shader);
//   window->vertex_shader = 0;
//   ID3D10Blob_Release(blob);
// }

#undef wpath_dirname
#undef wpath_join
#undef wpath_join_cwstr

#endif
