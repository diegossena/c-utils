#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window.h>
#include <sdk/application.h>
#include <sdk/error.h>
#include <windowsx.h>
#include <wincodec.h>
#include <winuser.h>
#define COBJMACROS 1
#include <d3d11.h>
#include <d3dcompiler.h>
#include <initguid.h>
#include <d2d1.h>
#include <dwrite.h>
#undef COBJMACROS

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
  task_t task;
  void* context;
  // window
  HWND handle;
  bool mouse_is_tracking;
  // events
  update_event_cb onupdate;
  mouse_event_cb onmousemove;
  mouse_event_cb onmousedown;
  mouse_event_cb onmouseup;
  mouse_event_cb ondblclick;
  keyboard_event_cb onkeydown;
  keyboard_event_cb onkeyup;
  ui_event_cb onresize;
  window_event_cb oncreate;
  window_event_cb onclose;
  // 3d_renderer
  IDXGISwapChain* swapchain;
  ID3D11Device* device;
  ID3D11DeviceContext* device_context;
  ID3D11RenderTargetView* backbuffer;
  ID3D11RasterizerState* rasterizer_state;
  ID3D11VertexShader* vertex_shader;
  ID3D11InputLayout* input_layout;
  ID3D11PixelShader* pixel_shader;
  // 2d_renderer
  ID2D1Factory* d2_factory;
  IDXGISurface* d2_surface;
  ID2D1RenderTarget* d2_render_target;
  IDWriteFactory* d2_write_factory;
} window_t;
typedef struct gfx_text_style_t {
  IDWriteTextFormat* data;
  f32 font_size;
} gfx_text_style_t;

u16 window_get_width(window_t* this) {
  RECT rect;
  GetWindowRect(this->handle, &rect);
  return rect.right - rect.left;
}
u16 window_get_height(window_t* this) {
  RECT rect;
  GetWindowRect(this->handle, &rect);
  return rect.bottom - rect.top;
}
void window_set_size(window_t* this, i32 width, i32 height) {
  RECT rect = { 0, 0, width, height };
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
  SetWindowPos(
    this->handle, null,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME
  );
}
vector2d_t window_get_cursor(window_t* this) {
  POINT point;
  GetCursorPos(&point);
  ScreenToClient(this->handle, &point);
  return (vector2d_t) { point.x, point.y };
}
void window_set_viewport(window_t* this, i32 width, i32 height) {
  if (this->backbuffer) {
    ID2D1RenderTarget_Release(this->d2_render_target);
    IDXGISurface_Release(this->d2_surface);
    ID3D11RenderTargetView_Release(this->backbuffer);
  }
  D3D11_VIEWPORT viewport = {
    .TopLeftX = 0.f,
    .TopLeftY = 0.f,
    .Width = (f32)width,
    .Height = (f32)height,
    .MinDepth = 0.f,
    .MaxDepth = 1.f,
  };
  ID3D11DeviceContext_RSSetViewports(this->device_context, 1, &viewport);
  IDXGISwapChain_ResizeBuffers(
    this->swapchain, 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0
  );
  // get backbuffer
  ID3D11Texture2D* backbuffer;
  IDXGISwapChain_GetBuffer(this->swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
  /**
   * use the back buffer address to create the render target
   * set the render target as the back buffer
   */
  ID3D11Device_CreateRenderTargetView(
    this->device, (ID3D11Resource*)backbuffer, null, &this->backbuffer
  );
  ID3D11DeviceContext_OMSetRenderTargets(
    this->device_context, 1, &this->backbuffer, 0
  );
  // set d2_render_target
  ID3D11Texture2D_QueryInterface(
    backbuffer, &IID_IDXGISurface, (void**)&this->d2_surface
  );
  D2D1_RENDER_TARGET_PROPERTIES renter_target_props = {
    .type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
    .pixelFormat = {
      .format = DXGI_FORMAT_UNKNOWN,
      .alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED
    }
  };
  HRESULT result = ID2D1Factory_CreateDxgiSurfaceRenderTarget(
    this->d2_factory, this->d2_surface, &renter_target_props,
    &this->d2_render_target
  );
  if (FAILED(result)) {
    error("CreateDxgiSurfaceRenderTarget", result);
  }
  // free resources
  ID3D11Texture2D_Release(backbuffer);
}

void window_free(window_t* this) {
  // D2D
  ID2D1RenderTarget_Release(this->d2_render_target);
  IDXGISurface_Release(this->d2_surface);
  ID2D1Factory_Release(this->d2_factory);
  // D3D
  if (this->input_layout) {
    ID3D11PixelShader_Release(this->pixel_shader);
    ID3D11InputLayout_Release(this->input_layout);
    ID3D11VertexShader_Release(this->vertex_shader);
  }
  ID3D11RasterizerState_Release(this->rasterizer_state);
  ID3D11RenderTargetView_Release(this->backbuffer);
  ID3D11DeviceContext_Release(this->device_context);
  ID3D11Device_Release(this->device);
  IDXGISwapChain_Release(this->swapchain);
  // window_t
  task_unregister(&this->task);
  this->handle = 0;
  memory_free(this);
}
void __window_mouse_tracking(window_t* this) {
  if (!this->mouse_is_tracking && this->onmousemove) {
    this->mouse_is_tracking = true;
    TRACKMOUSEEVENT tme = {
      .dwHoverTime = 800,
      .cbSize = sizeof(TRACKMOUSEEVENT),
      .dwFlags = TME_LEAVE,
      .hwndTrack = this->handle
    };
    TrackMouseEvent(&tme);
  }
}
vector2d_t __window_mouse_event_cursor_handler(LPARAM lParam) {
  return (vector2d_t) { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
}
void __window_update_handler(HWND handle, UINT unused1, UINT_PTR unused2, DWORD unused3) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  const FLOAT background_color [] = { 1.0f, 1.0f, 1.0f, 1.0f };
  ID3D11DeviceContext_ClearRenderTargetView(
    this->device_context, this->backbuffer, background_color
  );
  ID2D1RenderTarget_BeginDraw(this->d2_render_target);
  if (this->onupdate) {
    this->onupdate(this);
  }
  ID2D1RenderTarget_EndDraw(this->d2_render_target, null, null);
  IDXGISwapChain_Present(this->swapchain, 1, 0);
}
LRESULT __window_event_handler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_MOUSELEAVE:
      if (this->onmousemove) {
        this->mouse_is_tracking = false;
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
      DestroyWindow(handle);
      return 0;
    case WM_DESTROY:
      KillTimer(handle, 0);
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
  this->task.type = TASK_WINDOW;
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
  this->oncreate = options->oncreate;
  this->onclose = options->onclose;
  // CreateWindow
  RECT rect = { options->x, options->y, options->width, options->height };
  AdjustWindowRect(&rect, window_style, false);
  u32 width = rect.right - rect.left;
  u32 height = rect.bottom - rect.top;
  this->handle = CreateWindowExA(
    window_ex_style, wc.lpszClassName, options->name, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->handle) {
    error("CreateWindowExA", ERR_UNKNOWN);
    goto clear;
  }
  // renderer
  HRESULT result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, null,
    (void**)&this->d2_factory
  );
  if (FAILED(result)) {
    error("D2D1CreateFactory", result);
  }
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&this->d2_write_factory
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
  scd.OutputWindow = this->handle;                    // the window to be used
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
    &this->swapchain,
    &this->device,
    0,
    &this->device_context
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
  ID3D11Device_CreateRasterizerState(this->device, &rasterizer_desc, &this->rasterizer_state);
  // events
  if (this->oncreate) {
    this->oncreate(this);
  }
  __window_mouse_tracking(this);
  SetTimer(this->handle, 0, 1000 / 60, __window_update_handler);
  task_register(this, app);
  return;
clear:
  memory_free(this);
}

void window_pooling() {
  MSG msg;
  WINBOOL result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
  if (result) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
}
gfx_stroke_t* gfx_stroke_new(window_t* window, stroke_t props) {
  D2D1_STROKE_STYLE_PROPERTIES stroke_properties;
  stroke_properties.startCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.endCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.dashCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.lineJoin = D2D1_LINE_JOIN_ROUND;
  stroke_properties.miterLimit = 10.f;
  stroke_properties.dashStyle = D2D1_DASH_STYLE_SOLID;
  stroke_properties.dashOffset = 0.f;
  stroke_properties.startCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.endCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.dashCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.lineJoin = D2D1_LINE_JOIN_ROUND;
  stroke_properties.miterLimit = 10.f;
  stroke_properties.dashStyle = D2D1_DASH_STYLE_SOLID;
  stroke_properties.dashOffset = 0.f;
  ID2D1StrokeStyle* this;
  ID2D1Factory_CreateStrokeStyle(
    window->d2_factory, &stroke_properties, null, null,
    (ID2D1StrokeStyle**)&this
  );
  return this;
}
void gfx_stroke_free(gfx_stroke_t* this) {
  ID2D1StrokeStyle_Release((ID2D1StrokeStyle*)this);
}
void gfx_draw_rect(window_t* this, gfx_rect_t* props) {
  if (props->border_radius) {
    D2D1_ROUNDED_RECT rect = {
      {
        props->rect.left_top.x, props->rect.left_top.y,
        props->rect.right_bottom.x, props->rect.right_bottom.y
      },
      props->border_radius, props->border_radius
    };
    if (props->border_width >= 0) {
      ID2D1RenderTarget_DrawRoundedRectangle(
        this->d2_render_target, (D2D1_ROUNDED_RECT*)&rect, (ID2D1Brush*)props->color,
        props->border_width, (ID2D1StrokeStyle*)props->stroke
      );
    } else {
      ID2D1RenderTarget_FillRoundedRectangle(
        this->d2_render_target, (D2D1_ROUNDED_RECT*)&rect, (ID2D1Brush*)props->color
      );
    }
  } else if (props->border_width >= 0) {
    ID2D1RenderTarget_DrawRectangle(
      this->d2_render_target, (D2D1_RECT_F*)&props->rect, (ID2D1Brush*)props->color,
      props->border_width, (ID2D1StrokeStyle*)props->stroke
    );
  } else {
    ID2D1RenderTarget_FillRectangle(
      this->d2_render_target, (D2D1_RECT_F*)&props->rect, (ID2D1Brush*)props->color
    );
  }
}
void gfx_draw_fill_rect(window_t* this, gfx_rect_t* props) {
  ID2D1RenderTarget_FillRectangle(
    this->d2_render_target, (D2D1_RECT_F*)&props->rect,
    (ID2D1Brush*)props->color
  );
}
gfx_text_style_t* gfx_text_style_new(window_t* window, text_style_props_t props) {
  gfx_text_style_t* this = memory_alloc(sizeof(gfx_text_style_t));
  this->font_size = props.size;
  DWRITE_FONT_WEIGHT font_weight;
  DWRITE_FONT_STYLE font_style;
  switch (props.weight) {
    case FONT_WEIGHT_NORMAL:
      font_weight = DWRITE_FONT_WEIGHT_NORMAL;
      break;
    case FONT_WEIGHT_BOLD:
      font_weight = DWRITE_FONT_WEIGHT_BOLD;
      break;
  }
  switch (props.style) {
    case FONT_STYLE_NORMAL:
      font_style = DWRITE_FONT_STYLE_NORMAL;
      break;
  }
  IDWriteFactory_CreateTextFormat(
    window->d2_write_factory, props.family, null, font_weight, font_style,
    DWRITE_FONT_STRETCH_NORMAL, props.size, L"en-US", &this->data
  );
  return this;
}
void gfx_text_style_free(gfx_text_style_t* this) {
  IDWriteTextFormat_Release(this->data);
  memory_free(this);
}
gfx_color_t* gfx_color_new(window_t* window, color_t color) {
  ID2D1SolidColorBrush* this;
  ID2D1RenderTarget_CreateSolidColorBrush(
    window->d2_render_target, (D2D1_COLOR_F*)&color, null,
    (ID2D1SolidColorBrush**)&this
  );
  return this;
}
void gfx_color_free(gfx_color_t* this) {
  ID2D1SolidColorBrush_Release((ID2D1SolidColorBrush*)this);
}
void gfx_draw_text(const window_t* this, const gfx_text_t* props) {
  D2D1_RECT_F rect = { props->position.x, props->position.y };
  rect.bottom = props->style->font_size;
  rect.right = rect.bottom * props->length;
  ID2D1RenderTarget_DrawText(
    this->d2_render_target, props->text, props->length, props->style->data,
    &rect, (ID2D1Brush*)props->color, D2D1_DRAW_TEXT_OPTIONS_NONE,
    DWRITE_MEASURING_MODE_NATURAL
  );
}
bitmap_t* gfx_bitmap_new(window_t* window, const wchar_t* path) {
  ID2D1Bitmap* this;
  HRESULT result;
  IWICImagingFactory* wic_factory;
  IWICBitmapDecoder* decoder;
  IWICBitmapFrameDecode* frame_decode;
  IWICFormatConverter* converter;
  result = CoInitialize(NULL);
  if (FAILED(result)) {
    error("CoInitialize", result);
    return 0;
  }
  CLSID clsid = CLSID_WICImagingFactory;
  IID iid = IID_IWICImagingFactory;
  result = CoCreateInstance(
    &clsid, null, CLSCTX_INPROC_SERVER,
    &iid, (void**)&wic_factory
  );
  if (FAILED(result)) {
    error("CoCreateInstance", result);
    goto wic_factory_free;
  }
  result = wic_factory->lpVtbl->CreateDecoderFromFilename(
    wic_factory, path, NULL, GENERIC_READ,
    WICDecodeMetadataCacheOnDemand, &decoder
  );
  if (FAILED(result)) {
    error("CreateDecoderFromFilename", result);
    goto decoder_free;
  }
  result = decoder->lpVtbl->GetFrame(decoder, 0, &frame_decode);
  if (FAILED(result)) {
    error("GetFrame", result);
    goto decoder_free;
  }
  result = wic_factory->lpVtbl->CreateFormatConverter(wic_factory, &converter);
  if (FAILED(result)) {
    error("CreateFormatConverter", result);
    goto frame_decode_free;
  }
  result = converter->lpVtbl->Initialize(
    converter, (IWICBitmapSource*)frame_decode, &GUID_WICPixelFormat32bppPBGRA,
    WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeMedianCut
  );
  if (FAILED(result)) {
    error("IWICFormatConverter::Initialize", result);
    goto frame_decode_free;
  }
  result = window->d2_render_target->lpVtbl->CreateBitmapFromWicBitmap(
    window->d2_render_target, (IWICBitmapSource*)converter, NULL, &this
  );
  if (FAILED(result)) {
    error("CreateBitmapFromWicBitmap", result);
  }
frame_decode_free:
  frame_decode->lpVtbl->Release(frame_decode);
decoder_free:
  decoder->lpVtbl->Release(decoder);
wic_factory_free:
  wic_factory->lpVtbl->Release(wic_factory);
  return (bitmap_t*)this;
}
void gfx_bitmap_free(bitmap_t* this) {
  ID2D1Bitmap_Release((ID2D1Bitmap*)this);
}
void gfx_draw_bitmap(window_t* this, gfx_bitmap_t* props) {
  // draw
  (this->d2_render_target)->lpVtbl->DrawBitmap(
    this->d2_render_target, (ID2D1Bitmap*)props->image, (D2D1_RECT_F*)&props->rect, 1.f,
    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, null
  );
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
