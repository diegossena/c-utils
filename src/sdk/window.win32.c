#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS

void __window_onupdate(HWND handle, UINT message, window_t* this, u32 time_now) {
  // console_log("%x __window_onupdate message=%x time=%lu", this, message, time_now);
}
LRESULT __window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  window_t* this = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_PAINT:
      console_log("WM_PAINT");
      ID2D1HwndRenderTarget_BeginDraw(this->__d2d_render_target);
      D2D1_COLOR_F color = { 1.f, 0.f, 0.f, 1.f };
      ID2D1HwndRenderTarget_Clear(this->__d2d_render_target, &color);
      ID2D1HwndRenderTarget_EndDraw(this->__d2d_render_target, 0, 0);
      break;
    case WM_CREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      this = (window_t*)pcs->lpCreateParams;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)this);
      return 0;
    };
    case WM_CLOSE:
      KillTimer(handle, 0);
      break;
    case WM_DESTROY:
      IDWriteFactory_Release(this->__d2d_write_factory);
      ID2D1HwndRenderTarget_Release(this->__d2d_render_target);
      ID2D1Factory_Release(this->__d2d_factory);
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcA(handle, message, wParam, lParam);
}

void window_redraw(window_t* this) {
  RedrawWindow(this->__handle, 0, 0, RDW_INVALIDATE);
}
void window_constructor(
  window_t* this, const char* title,
  i32 width, i32 height
) {
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
  // window_create
  RECT rect = { 0, 0, width, height };
  AdjustWindowRect(&rect, window_style, false);
  this->__handle = CreateWindowExA(
    window_ex_style, wc.lpszClassName, title, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->__handle) {
    return error_log("CreateWindowExA", ERR_UNKNOWN);
  }
  // d2d.factory
  HRESULT result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, 0,
    (void**)&this->__d2d_factory
  );
  if (FAILED(result)) {
    error_log("D2D1CreateFactory", result);
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
    .hwnd = this->__handle,
    .pixelSize = { width, height },
    .presentOptions = D2D1_PRESENT_OPTIONS_NONE
  };
  result = ID2D1Factory_CreateHwndRenderTarget(
    this->__d2d_factory,
    &render_target_props,
    &window_render_target_props,
    &this->__d2d_render_target
  );
  if (FAILED(result)) {
    error_log("CreateHwndRenderTarget", result);
    goto onerror;
  }
  // d2d.write_factory
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&this->__d2d_write_factory
  );
  if (FAILED(result)) {
    error_log("DWriteCreateFactory", result);
  }
  // onsuccess
  SetTimer(this->__handle, (UINT_PTR)this, 0, (TIMERPROC)__window_onupdate);
  return;
onerror:
  return;
}

#endif