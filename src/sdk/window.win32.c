#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

HWND global_window;
ID2D1Factory* global_d2d_factory;
ID2D1RenderTarget* global_d2d_render_target;
IDWriteFactory* global_dwrite_factory;
IDWriteFontCollection* global_dwrite_collection = 0;

export void window_clear() {
  ID2D1RenderTarget_Clear(global_d2d_render_target, 0);
}
export void window_close() {
  DestroyWindow(global_window);
}
export void __window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (global_window_focus && global_window_keyboard_count) {
    window_onkeypress();
  }
}
LRESULT __window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    // case WM_PAINT:
    //   ID2D1RenderTarget_BeginDraw(global_d2d_render_target);
    //   window_onrender();
    //   ID2D1RenderTarget_EndDraw(global_d2d_render_target, 0, 0);
    //   break;
    case WM_KEYDOWN:
      window_onkeydown(wParam);
      if (wParam != 91 && !window_key_pressed(wParam)) {
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        global_window_keyboard_state[byte_index] |= (1 << bit_index);
        ++global_window_keyboard_count;
      }
      return 0;
    case WM_KEYUP: {
      window_onkeyup();
      u8 byte_index = wParam / 8;
      u8 bit_index = wParam % 8;
      global_window_keyboard_state[byte_index] &= ~(1 << bit_index);
      --global_window_keyboard_count;
      return 0;
    }
    case WM_SETFOCUS:
      global_window_focus = true;
      return 0;
    case WM_KILLFOCUS:
      global_window_focus = false;
      return 0;
    case WM_DESTROY:
      KillTimer(0, 0);
      thread_free(global_window_thread);
      global_window_thread = 0;
      IDWriteFactory_Release(global_dwrite_factory);
      ID2D1RenderTarget_Release(global_d2d_render_target);
      ID2D1Factory_Release(global_d2d_factory);
      IDWriteFontCollection_Release(global_dwrite_collection);
      if (global_dwrite_collection) {
        global_dwrite_collection->lpVtbl->Release(global_dwrite_collection);
      }
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcA(handle, message, wParam, lParam);
}
export void window_set_title(const char* title) {
  SetWindowTextA(global_window, title);
}
export void window_redraw() {
  ID2D1RenderTarget_BeginDraw(global_d2d_render_target);
  window_onrender();
  ID2D1RenderTarget_EndDraw(global_d2d_render_target, 0, 0);
}
export void window_fill_rectangle(
  f32 left, f32 top, f32 right, f32 bottom,
  f32 r, f32 g, f32 b, f32 a
) {
  D2D1_RECT_F rect = { left, top, right, bottom };
  ID2D1SolidColorBrush* brush;
  D2D1_COLOR_F color = { r, g, b, a };
  ID2D1RenderTarget_CreateSolidColorBrush(global_d2d_render_target, &color, 0, &brush);
  ID2D1RenderTarget_FillRectangle(global_d2d_render_target, &rect, (ID2D1Brush*)brush);
  ID2D1SolidColorBrush_Release(brush);
}
void __window_thread() {
  i32 result;
  const char* title = " ";
  // window_class_register
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = __window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .lpszClassName = title,
  };
  result = RegisterClassExA(&wc);
  if (!result) {
    console_log("RegisterClassExA %d %s", result, error_cstr(result));
  }
  u32 window_style = WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // d2d.factory
  result = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, 0,
    (void**)&global_d2d_factory
  );
  if (FAILED(result)) {
    console_log("D2D1CreateFactory %d %s", result, error_cstr(result));
  }
  // d2d.write_factory
  result = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
    (IUnknown**)&global_dwrite_factory
  );
  if (FAILED(result)) {
    console_log("DWriteCreateFactory %d %s", result, error_cstr(result));
  }
  // window_create
  RECT rect = { 0, 0, global_window_width, global_window_height };
  AdjustWindowRect(&rect, window_style, false);
  global_window = CreateWindowExA(
    window_ex_style, wc.lpszClassName, title, window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    0 // pointer to window-creation data
  );
  if (!global_window) {
    console_log("CreateWindowExA", global_window, error_cstr((error_t)global_window));
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
    .hwnd = global_window,
    .pixelSize = { global_window_width, global_window_height },
    .presentOptions = D2D1_PRESENT_OPTIONS_NONE
  };
  result = ID2D1Factory_CreateHwndRenderTarget(
    global_d2d_factory,
    &render_target_props,
    &window_render_target_props,
    (ID2D1HwndRenderTarget**)&global_d2d_render_target
  );
  if (FAILED(result)) {
    console_log("CreateHwndRenderTarget", result, error_cstr(result));
    goto onerror;
  }
  sync_signal(global_window_onload_sync);
  SetTimer(0, 0, 0, (TIMERPROC)__window_onupdate);
  MSG msg;
  while (true) {
    MsgWaitForMultipleObjectsEx(
      0,
      0,
      INFINITE,
      QS_ALLINPUT,
      MWMO_INPUTAVAILABLE
    );
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        return;
      }
    }
  }
onerror:
  return;
}

#endif