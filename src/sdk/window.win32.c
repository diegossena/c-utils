#include <sdk/window.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>
#include <sdk/time.h>
#include <sdk/memory.h>

#include <avrt.h>

HWND _window_id;

void _window_onupdate(void* _1, void* _2, void* _3, u32 time) {
  if (window_focus && _keyboard_count) {
    window_onkeypress();
  }
}
LRESULT _window_procedure(HWND window_id, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT:
      ValidateRect(window_id, 0);
      break;
    case WM_KEYDOWN:
      if (window_focus) {
        if (wParam != 91 && !window_key_pressed(wParam)) {
          u8 byte_index = wParam / 8;
          u8 bit_index = wParam % 8;
          _keyboard_state[byte_index] |= (1 << bit_index);
          ++_keyboard_count;
          window_onkeydown(wParam);
        }
      }
      return 0;
    case WM_KEYUP:
      if (window_focus) {
        u8 byte_index = wParam / 8;
        u8 bit_index = wParam % 8;
        _keyboard_state[byte_index] &= ~(1 << bit_index);
        --_keyboard_count;
        window_onkeyup(wParam);
      }
      return 0;
    case WM_MOUSEMOVE:
      mouse_x = GET_X_LPARAM(lParam);
      mouse_y = GET_Y_LPARAM(lParam);
      window_onmousemove();
      return 0;
    case WM_MOUSELEAVE:
      return 0;
    case WM_LBUTTONDOWN:
      window_onmousedown(MOUSE_BUTTON_LEFT);
      return 0;
    case WM_RBUTTONDOWN:
      window_onmousedown(MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_MBUTTONDOWN:
      window_onmousedown(MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_XBUTTONDOWN:
      window_onmousedown(MOUSE_BUTTON_AUX);
      return 0;
    case WM_LBUTTONUP:
      window_onmouseup(MOUSE_BUTTON_LEFT);
      return 0;
    case WM_RBUTTONUP:
      window_onmouseup(MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_MBUTTONUP:
      window_onmousedown(MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_XBUTTONUP:
      window_onmouseup(MOUSE_BUTTON_AUX);
      return 0;
    case WM_LBUTTONDBLCLK:
      window_dblclick();
      return 0;
    case WM_MOUSEWHEEL:
      window_onscroll(GET_WHEEL_DELTA_WPARAM(wParam));
      return 0;
    case WM_SIZE:
      if (_render_time) {
        window_resized = true;
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
      }
      break;
    case WM_SETFOCUS:
      window_focus = true;
      return 0;
    case WM_KILLFOCUS:
      window_focus = false;
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcA(window_id, message, wParam, lParam);
}
void window_close() { DestroyWindow(_window_id); }
void window_startup(const char* title, const char* atlas_path, u16 atlas_width, u16 atlas_height) {
  SetProcessDPIAware();
  // window_class_register
  WNDCLASSEXA window_class = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = _window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .lpszClassName = title,
  };
  i32 result = RegisterClassExA(&window_class);
  if (!result) {
    error(result, "RegisterClassExA");
  }
  u32 window_style = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
  u32 window_ex_style = WS_EX_APPWINDOW;
  // CreateWindowExA
  RECT rect = { 0, 0, window_width, window_height };
  AdjustWindowRectEx(&rect, window_style, false, window_ex_style);
  _window_id = CreateWindowExA(
    window_ex_style, window_class.lpszClassName, title, window_style,
    0, 0, rect.right - rect.left, rect.bottom - rect.top,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    window_class.hInstance,
    0 // pointer to window-creation data
  );
  if (!_window_id) {
    error((error_t)_window_id, "CreateWindowExA");
  }
  _gfx_inicialize(atlas_path, atlas_width, atlas_height);
  _window_resize();
}
void window_set_title(const char* title) {
  assert(_window_id != 0);
  SetWindowTextA(_window_id, title);
}
void window_run() {
  // renderer
  timeBeginPeriod(1);
  DWORD task_index = 0;
  HANDLE mmtask = AvSetMmThreadCharacteristicsA("Games", &task_index);
  if (!mmtask) {
    error(GetLastError(), "window_run AvSetMmThreadCharacteristicsA");
  }
  HANDLE renderer_timer;
  _render_time = time_now_f64();
  CreateTimerQueueTimer(&renderer_timer, 0, (WAITORTIMERCALLBACK)_window_render, 0, 0, 15, 0);
  // loop
  SetTimer(0, 0, 0, (TIMERPROC)_window_onupdate);
  do {
    MsgWaitForMultipleObjectsEx(
      0,
      0,
      INFINITE,
      QS_ALLINPUT,
      MWMO_INPUTAVAILABLE
    );
    MSG msg;
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        timeEndPeriod(1);
        AvRevertMmThreadCharacteristics(mmtask);
        DeleteTimerQueueTimer(0, renderer_timer, INVALID_HANDLE_VALUE);
        KillTimer(0, 0);
        _gfx_destroy();
        return;
      }
    }
  } while (true);
}

#endif