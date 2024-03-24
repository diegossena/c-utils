#include <sdk/platform.h>

#ifdef PLATFORM_WINDOWS

#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/error.h>
#include <sdk/time.h>

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
  queue_t onkeypress;
  queue_t onkeydown;
  queue_t onkeyup;
  queue_t onresize;
  queue_t onclose;
  // window
  HWND __hwnd;
  bool __mouse_tracking;
  // fonts
  queue_t __fonts; // __font_queue_t
  IDWriteFontCollection* __collection;
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

#include <sdk/window/gfx/directdraw/FontCollectionLoader.win32.h>

static bool __window_running = false;

void window_set_size(window_t* this, u16 width, u16 height) {
  RECT rect = { 0, 0, width, height };
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
  SetWindowPos(
    this->__hwnd, null,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME
  );
}
void window_free(window_t* this) {
  // fonts
  if (this->__collection) {
    IDWriteFontCollection_Release(this->__collection);
  }
  // d2d
  ID2D1RenderTarget_Release(this->__d2d_render_target);
  IDXGISurface_Release(this->__d2d_surface);
  ID2D1Factory_Release(this->__d2d_factory);
  // d3d
  if (this->__d3d_input_layout) {
    ID3D11PixelShader_Release(this->__d3d_pixel_shader);
    ID3D11InputLayout_Release(this->__d3d_input_layout);
    ID3D11VertexShader_Release(this->__d3d_vertex_shader);
  }
  ID3D11RasterizerState_Release(this->__d3d_rasterizer);
  ID3D11RenderTargetView_Release(this->__d3d_backbuffer);
  ID3D11DeviceContext_Release(this->__d3d_device_context);
  ID3D11Device_Release(this->__d3d_device);
  IDXGISwapChain_Release(this->__d3d_swapchain);
  // window_t
  KillTimer(this->__hwnd, 0);
  DestroyWindow(this->__hwnd);
  memory_free(this);
}
void __window_mouse_tracking(window_t* this) {
  if (!this->__mouse_tracking) {
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
void __window_update_callback(HWND handle, UINT unused1, UINT_PTR unused2, DWORD unused3) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  f64 now = time_absolute();
  this->elapsed_time = now - this->__updated_at;
  this->__updated_at = now;
  if (this->has_focus && __keyboard_count) {
    emitter_emit(&this->onkeypress);
  }
  emitter_emit(&this->onupdate);
}
void window_render_request(window_t* this) {
  RedrawWindow(this->__hwnd, NULL, NULL, RDW_INVALIDATE);
}
LRESULT __window_event_handler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_PAINT:
      if (!queue_empty(&this->ondraw)) {
        static const FLOAT background_color [] = { 0.0f, 0.0f, 0.0f, 1.0f };
        ID3D11DeviceContext_ClearRenderTargetView(
          this->__d3d_device_context, this->__d3d_backbuffer, background_color
        );
        ID2D1RenderTarget_BeginDraw(this->__d2d_render_target);
        emitter_emit(&this->ondraw);
        ID2D1RenderTarget_EndDraw(this->__d2d_render_target, null, null);
        IDXGISwapChain_Present(this->__d3d_swapchain, 0, 0);
      }
      break;
    case WM_MOUSELEAVE:
      if (!queue_empty(&this->ondraw)) {
        this->__mouse_tracking = false;
        this->cursor = (vector2d_t) { -1, -1 };
        emitter_emit(&this->onmousemove);
      }
      return 0;
    case WM_MOUSEMOVE:
      if (!queue_empty(&this->onmousemove)) {
        __window_mouse_tracking(this);
        this->cursor = (vector2d_t) { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        emitter_emit(&this->onmousemove);
      }
      return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
      emitter_emit(&this->onmousedown);
      return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
      emitter_emit(&this->onmouseup);
      return 0;
    case WM_LBUTTONDBLCLK:
      emitter_emit(&this->ondblclick);
      return 0;
    case WM_KEYDOWN:
      if (!keyboard_pressed(wParam)) {
        __keyboard_state[wParam % 8] |= wParam;
        ++__keyboard_count;
      }
      if (this->has_focus) {
        emitter_emit(&this->onkeydown);
      }
      return 0;
    case WM_KEYUP:
      __keyboard_state[wParam % 8] |= wParam;
      --__keyboard_count;
      if (this->has_focus) {
        emitter_emit(&this->onkeyup);
      }
      return 0;
    case WM_SETFOCUS:
      this->has_focus = true;
      return 0;
    case WM_KILLFOCUS:
      this->has_focus = false;
      return 0;
    case WM_SIZE: {
      this->width = LOWORD(lParam);
      this->height = HIWORD(lParam);
      // viewport
      HRESULT result;
      if (this->__d3d_backbuffer) {
        ID2D1RenderTarget_Release(this->__d2d_render_target);
        IDXGISurface_Release(this->__d2d_surface);
        ID3D11RenderTargetView_Release(this->__d3d_backbuffer);
      }
      D3D11_VIEWPORT viewport = {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = (f32)this->width,
        .Height = (f32)this->height,
        .MinDepth = 0.f,
        .MaxDepth = 1.f,
      };
      ID3D11DeviceContext_RSSetViewports(this->__d3d_device_context, 1, &viewport);
      result = IDXGISwapChain_ResizeBuffers(
        this->__d3d_swapchain, 1, this->width, this->height, DXGI_FORMAT_R8G8B8A8_UNORM, 0
      );
      if (FAILED(result)) {
        error("IDXGISwapChain_ResizeBuffers", result);
        return false;
      }
      // get backbuffer
      ID3D11Texture2D* backbuffer;
      IDXGISwapChain_GetBuffer(this->__d3d_swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
      /**
       * use the back buffer address to create the render target
       * set the render target as the back buffer
       */
      ID3D11Device_CreateRenderTargetView(
        this->__d3d_device, (ID3D11Resource*)backbuffer, null, &this->__d3d_backbuffer
      );
      ID3D11DeviceContext_OMSetRenderTargets(
        this->__d3d_device_context, 1, &this->__d3d_backbuffer, 0
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
      result = ID2D1Factory_CreateDxgiSurfaceRenderTarget(
        this->__d2d_factory, this->__d2d_surface, &renter_target_props,
        &this->__d2d_render_target
      );
      if (FAILED(result)) {
        error("CreateDxgiSurfaceRenderTarget", result);
      }
      // free resources
      ID3D11Texture2D_Release(backbuffer);
      if (!queue_empty(&this->onresize)) {
        emitter_emit(&this->onresize);
      }
      return 0;
    }
    case WM_CREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      this = (window_t*)pcs->lpCreateParams;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)this);
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
      scd.BufferDesc.Width = this->width;                 // set the back buffer width
      scd.BufferDesc.Height = this->height;               // set the back buffer height
      scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
      scd.OutputWindow = handle;                          // the window to be used
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
        &this->__d3d_swapchain,
        &this->__d3d_device,
        0,
        &this->__d3d_device_context
      );
      if (FAILED(result)) {
        error("D3D11CreateDeviceAndSwapChain", result);
        exit(1);
      }
      D3D11_RASTERIZER_DESC rasterizer_props = {
        .FillMode = D3D11_FILL_SOLID,
        .CullMode = D3D11_CULL_NONE
      };
      result = ID3D11Device_CreateRasterizerState(
        this->__d3d_device, &rasterizer_props, &this->__d3d_rasterizer
      );
      if (FAILED(result)) {
        error("ID3D11Device_CreateRasterizerState", result);
      }
      return 0;
    };
    case WM_CLOSE:
      emitter_emit(&this->onclose);
      window_free(this);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProc(handle, message, wParam, lParam);
}

void window_startup(application_t* app, window_props_t* options) {
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
   // properties
  this->width = options->width;
  this->height = options->height;
  this->__updated_at = time_absolute();
  // event_listener_t
  queue_head(&this->onupdate);
  queue_head(&this->ondraw);
  queue_head(&this->onmousemove);
  queue_head(&this->onmousedown);
  queue_head(&this->onmouseup);
  queue_head(&this->onkeypress);
  queue_head(&this->onkeydown);
  queue_head(&this->onclose);
  queue_head(&this->ondblclick);
  queue_head(&this->onkeyup);
  queue_head(&this->onresize);
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
    goto window_release;
  }
  u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
  if (!(options->flags & WINDOW_FLAG_HIDDEN)) {
    window_style |= WS_VISIBLE;
  }
  if (!(options->flags & WINDOW_FLAG_NO_RESIZABLE)) {
    window_style |= WS_THICKFRAME;
  }
  if (!(options->flags & WINDOW_FLAG_NO_MAXIMIZE)) {
    window_style |= WS_MAXIMIZEBOX;
  }
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindow
  RECT rect = { 0, 0, options->width, options->height };
  AdjustWindowRect(&rect, window_style, false);
  this->__hwnd = CreateWindowExA(
    window_ex_style, wc.lpszClassName, options->name, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->__hwnd) {
    error("CreateWindowExA", ERR_UNKNOWN);
    goto window_release;
  }
  // preload
  if (options->onpreload) {
    queue_head(&this->__fonts);
    options->onpreload(this);
    if (this->__fonts.next != &this->__fonts) {
      __FontCollectionLoader collection_loader;
      __FontCollectionLoader_Inicialize(&collection_loader, &this->__fonts);
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
      queue_foreach(__font_queue_t, this->__fonts) {
        queue_remove((queue_t*)it);
        memory_free(it);
      }
    } else {
      this->__collection = 0;
    }
  }
  // load
  if (options->onload) {
    options->onload(this);
  }
  SetTimer(this->__hwnd, 0, 1000 / 60, __window_update_callback);
  __window_running = true;
  return;
d2d_factory_release:
  IDWriteFactory_Release(this->__d2d_write_factory);
window_release:
  memory_free(this);
}

bool window_pooling() {
  MSG msg;
  WINBOOL result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
  if (result) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      __window_running = false;
    }
  }
  return __window_running;
}

#undef wpath_dirname
#undef wpath_join
#undef wpath_join_cwstr

#endif
