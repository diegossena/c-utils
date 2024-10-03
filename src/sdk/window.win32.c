#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS

#define COBJMACROS 1
#include <initguid.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <windows.h>

HWND __sdk_window_handle = 0;
ID2D1Factory* __sdk_d2d_factory;
ID2D1HwndRenderTarget* __sdk_d2d_render_target;
IDWriteFactory* __sdk_d2d_write_factory;

void window_run() {
  MSG msg;
  i32 result;
  while (_sdk_window_running) {
    result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
    if (result) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        _sdk_window_running = false;
      }
    }
  }
}
void __window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  window_onupdate(time);
}
D2D1_RECT_F rect = {
  .top = 0.f,
  .bottom = 10.f,
  .left = 0.f,
  .right = 10.f
};
ID2D1SolidColorBrush* brush;
ID2D1StrokeStyle* stroke;
LRESULT __window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT:
      ID2D1HwndRenderTarget_BeginDraw(__sdk_d2d_render_target);
      ID2D1HwndRenderTarget_Clear(__sdk_d2d_render_target, 0);
      // ID2D1HwndRenderTarget_DrawRectangle(__d2d_render_target, &rect, (ID2D1Brush*)brush, 1.f, stroke);
      ID2D1HwndRenderTarget_EndDraw(__sdk_d2d_render_target, 0, 0);
      break;
    case WM_CLOSE:
      KillTimer(0, 0);
      break;
    case WM_DESTROY:
      // ID2D1StrokeStyle_Release(stroke);
      // ID2D1SolidColorBrush_Release(brush);
      IDWriteFactory_Release(__sdk_d2d_write_factory);
      ID2D1HwndRenderTarget_Release(__sdk_d2d_render_target);
      ID2D1Factory_Release(__sdk_d2d_factory);
      PostQuitMessage(0);
      _sdk_window_running = false;
      return 0;
  }
  return DefWindowProcA(handle, message, wParam, lParam);
}

void window_redraw() {
  RedrawWindow(__sdk_window_handle, 0, 0, RDW_INVALIDATE);
}
void window_startup(
  const char* title,
  i32 width, i32 height
) {
  assert(_sdk_window_running == false);
  _sdk_window_running = true;
  LPCSTR class_name = "SDK_WINDOW";
  // window_class_register
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = __window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .lpszClassName = class_name,
  };
  if (!RegisterClassExA(&wc)) {
    return error_log("RegisterClassExA", ERR_UNKNOWN);
  }
  u32 window_style = WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // d2d.factory
  HRESULT result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, 0,
    (void**)&__sdk_d2d_factory
  );
  if (FAILED(result)) {
    error_log("D2D1CreateFactory", result);
  }
  // d2d.write_factory
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&__sdk_d2d_write_factory
  );
  if (FAILED(result)) {
    error_log("DWriteCreateFactory", result);
  }
  // brush
  D2D1_COLOR_F brush_color = { 1.f, 0, 0, 1.f };
  D2D1_BRUSH_PROPERTIES brush_props = { .opacity = 1.f };
  // ID2D1HwndRenderTarget_CreateSolidColorBrush(__d2d_render_target, &brush_color, &brush_props, &brush);
  // stroke
  D2D1_STROKE_STYLE_PROPERTIES stroke_props = {};
  // ID2D1Factory_CreateStrokeStyle(__d2d_factory, &stroke_props, 0, 0, &stroke);
  // window_create
  RECT rect = { 0, 0, width, height };
  AdjustWindowRect(&rect, window_style, false);
  __sdk_window_handle = CreateWindowExA(
    window_ex_style, wc.lpszClassName, title, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    0 // pointer to window-creation data
  );
  if (!__sdk_window_handle) {
    return error_log("CreateWindowExA", ERR_UNKNOWN);
  }
  // d2d.render_target
  D2D1_RENDER_TARGET_PROPERTIES render_target_props = {
    .type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
    .pixelFormat = {
      .format = DXGI_FORMAT_UNKNOWN,
      .alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED
    }
  };
  D2D1_HWND_RENDER_TARGET_PROPERTIES window_render_target_props = {
    .hwnd = __sdk_window_handle,
    .pixelSize = { width, height },
    .presentOptions = D2D1_PRESENT_OPTIONS_NONE
  };
  result = ID2D1Factory_CreateHwndRenderTarget(
    __sdk_d2d_factory,
    &render_target_props,
    &window_render_target_props,
    &__sdk_d2d_render_target
  );
  if (FAILED(result)) {
    error_log("CreateHwndRenderTarget", result);
    goto onerror;
  }
  // onsuccess
  SetTimer(0, 0, 0, (TIMERPROC)__window_onupdate);
  return;
onerror:
  return;
}

#endif