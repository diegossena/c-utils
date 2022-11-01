#pragma once

#include <tchar.h>
#include <windows.h>
#include <stdbool.h> 
#include "String_t.h"
#include "smart_ptr.h"

// definitions //

typedef enum WindowFlags {
  WINDOW_HIDDEN = 0x1,
  WINDOW_RESIZABLE = 0x2,
  WINDOW_BORDERLESS = 0x4,
} WindowFlags;

typedef enum WindowEvent {
  WINDOW_QUIT = WM_QUIT,
  WINDOW_UNDEFINED = WM_TIMER,
  WINDOW_FOCUS = WM_SETFOCUS,
  WINDOW_BLUR = WM_KILLFOCUS,
  WINDOW_RESIZE = WM_SIZE,
  WINDOW_MOUSE_MOVE = WM_MOUSEMOVE
} WindowEvent;

// type //

typedef struct Window {
  HWND id;
  WindowEvent event;
  LPARAM lParam;
} Window;
typedef void (*WindowCallback)(Window* window);

// handlers //

LRESULT CALLBACK wndProc(
  HWND hWnd,
  UINT message,
  WPARAM wParam,
  LPARAM lParam
) {

  switch (message) {

  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_CREATE: {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    WindowCallback callback = (WindowCallback)pcs->lpCreateParams;
    SetWindowLongPtrA(
      hWnd,
      GWLP_USERDATA,
      (LONG_PTR)callback
    );
    break;

  }

  }

  WindowCallback callback = (WindowCallback)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
  if (callback) {
    Window window = {
      .id = hWnd,
      .event = message,
      .lParam = lParam
    };
    callback(&window);
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

// constructors //

void window_new(
  WindowCallback window_callback,
  const char* title,
  unsigned int x, unsigned int y,
  unsigned int width, unsigned int height,
  WindowFlags flags
) {
  size_t dwStyle = flags & WINDOW_BORDERLESS
    ? WS_POPUP
    : WS_SYSMENU | WS_MINIMIZEBOX;
  if (!(flags & WINDOW_HIDDEN))
    dwStyle |= WS_VISIBLE;
  if (flags & WINDOW_RESIZABLE)
    dwStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
  WNDCLASSEX wc = {
    .cbSize = sizeof(WNDCLASSEX),
    .lpfnWndProc = wndProc,
    .lpszClassName = title,
    .hCursor = LoadCursor(0, IDC_ARROW),
  };
  RegisterClassEx(&wc);
  HWND hWnd = CreateWindowEx(
    0, // extended window style
    wc.lpszClassName, // pointer to registered class name
    wc.lpszClassName, // pointer to window name
    dwStyle, // window style
    x, // horizontal position of window
    y, // vertical position of window
    width, // window width
    height, // window height
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance, // handle to application instance
    window_callback // pointer to window-creation data
  );
  SetTimer(hWnd, 0, USER_TIMER_MINIMUM, NULL);
}
void window_new_centered(
  WindowCallback callback,
  const char* title,
  unsigned int width, unsigned int height,
  WindowFlags flags
) {
  window_new(
    callback, title,
    (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2),
    width, height,
    flags
  );
}
void window_new_fullscreen(WindowCallback callback, const char* title, WindowFlags flags) {
  window_new(
    callback, title, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
    flags | WINDOW_BORDERLESS
  );
}

// methods //

void windows_run() {
  MSG msg;
  while (true) {
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT)
        return;
    }
  }
}

WindowEvent window_event_get(Window* window) { return window->event; }
WORD window_mouse_get_x(Window* window) { return LOWORD(window->lParam); }
WORD window_mouse_get_y(Window* window) { return HIWORD(window->lParam); }
void window_show(Window* window) { ShowWindow(window->id, SW_SHOWDEFAULT); }
void window_hide(Window* window) { ShowWindow(window->id, SW_HIDE); }
void window_set_caption(Window* window, const char* title) { SetWindowText(window->id, title); }
void window_close(Window* window) { CloseWindow(window->id); }
void window_destroy(Window* window) { DestroyWindow(window->id); }

int screen_get_width() { return GetSystemMetrics(SM_CXSCREEN); }
int screen_get_height() { return GetSystemMetrics(SM_CYSCREEN); }