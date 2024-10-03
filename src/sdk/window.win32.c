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
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        _sdk_window_running = false;
      }
    }
    Sleep(16);
  }
}
u32 updated_at = 0;
void __window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  console_log("time %llu", time - updated_at);
  updated_at = time;
  window_onupdate(time);
}
LRESULT __window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT:
      ID2D1HwndRenderTarget_BeginDraw(__sdk_d2d_render_target);
      ID2D1HwndRenderTarget_Clear(__sdk_d2d_render_target, 0);
      window_onrender();
      ID2D1HwndRenderTarget_EndDraw(__sdk_d2d_render_target, 0, 0);
      break;
    case WM_CLOSE:
      KillTimer(0, 0);
      break;
    case WM_DESTROY:
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
void window_fill_rectangle(
  f32 left, f32 top, f32 right, f32 bottom,
  f32 r, f32 g, f32 b, f32 a
) {
  D2D1_RECT_F rect = { left, top, right, bottom };
  ID2D1SolidColorBrush* brush;
  D2D1_COLOR_F color = { r, g, b, a };
  ID2D1HwndRenderTarget_CreateSolidColorBrush(__sdk_d2d_render_target, &color, 0, &brush);
  ID2D1HwndRenderTarget_FillRectangle(__sdk_d2d_render_target, &rect, (ID2D1Brush*)brush);
  ID2D1SolidColorBrush_Release(brush);
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
  SetTimer(0, 0, 16, (TIMERPROC)__window_onupdate);
  return;
onerror:
  return;
}

#endif