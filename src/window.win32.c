#include "internal/platform.h"

#if PLATFORM_WINDOWS

// window
#include "internal/window.win32.h"
// memory_alloc
#include "internal/memory.h"
// error
#include "sdk/error.h"
// app_main
#include "internal/application.h"
// console_log_cstr
#include "sdk/console.h"

LRESULT window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  // console_log("WINDOW_EVENT=[%x, %llu]", message, message);
  window_t* window = (window_t*)GetWindowLongPtrA(handle, GWLP_USERDATA);
  switch (message) {
    case WM_TIMER:
      break;
    case WM_ERASEBKGND: {
      HDC hdc = (HDC)wParam;
      RECT rect;
      GetClientRect(handle, &rect);
      HBRUSH white_background = (HBRUSH)(COLOR_WINDOW + 1);
      FillRect(hdc, &rect, white_background); // Mesma cor definida na classe da janela
      return 1; // Indicar que o fundo foi desenhado
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(handle, &ps);
      // Desenhar texto na janela
      TextOut(hdc, 10, 10, "Hello, World!", 13);
      EndPaint(handle, &ps);
    } break;
    case WM_NCCREATE: {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      window = (window_t*)pcs->lpCreateParams;
      SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)window);
    } break;
    case WM_CLOSE: // onClose
      DestroyWindow(handle);
      window->handle = 0;
      break;
    case WM_DESTROY: // onDestroy
      KillTimer(handle, 0);
      window->handle = 0;
      memory_free(window);
      PostQuitMessage(0);
      break;
  }
  return DefWindowProc(handle, message, wParam, lParam);
}

window_t* window_new(window_opt* options) {
  window_t* this = memory_alloc(sizeof(window_t));
  // RegisterWindowClass
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = window_procedure,
    .hInstance = GetModuleHandleA(0),
    .hCursor = LoadCursor(0, IDC_ARROW),
    .lpszClassName = "MainWClass"
  };
  if (!RegisterClassExA(&wc)) {
    error("RegisterClassExA", ERR_UNKNOWN);
    goto clear;
  }
  u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
  if (!(options->flags & WINDOW_HIDDEN)) {
    window_style |= WS_VISIBLE;
  }
  u32 window_ex_style = WS_EX_APPWINDOW;
  this->handle = CreateWindowExA(
    window_ex_style, wc.lpszClassName, options->name, window_style,
    options->x, options->y, options->width, options->height,
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance,
    this // pointer to window-creation data
  );
  if (!this->handle) {
    error("CreateWindowExA", ERR_UNKNOWN);
    goto clear;
  }
  SetTimer(this->handle, 0, 0, 0);
  ++app_global.tasks_count;
  return this;
clear:
  memory_free(this);
  return 0;
}

#endif