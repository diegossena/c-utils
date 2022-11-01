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
  WINDOW_QUIT = 0,
  WINDOW_UNDEFINED
} WindowEvent;

// type //

#define Window HWND
typedef void (*WindowCallback)(Window, UINT, WPARAM, LPARAM);

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
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_CREATE: {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    WindowCallback callback = (WindowCallback)pcs->lpCreateParams;
    SetWindowLongPtrA(
      hWnd,
      GWLP_USERDATA,
      (LONG_PTR)callback
    );
    return 0;
  }

  }
  WindowCallback callback = (WindowCallback)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
  if (callback)
    callback(hWnd, message, wParam, lParam);

  return DefWindowProc(hWnd, message, wParam, lParam);
}

// constructors //

Window window_new(
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
  return hWnd;
}
Window window_new_centered(
  WindowCallback callback,
  const char* title,
  unsigned int width, unsigned int height,
  WindowFlags flags
) {
  return window_new(
    callback, title,
    (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2),
    width, height,
    flags
  );
}
Window window_new_fullscreen(WindowCallback callback, const char* title, WindowFlags flags) {
  return window_new(
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

void window_show(Window window) { ShowWindow(window, SW_SHOWDEFAULT); }
void window_hide(Window window) { ShowWindow(window, SW_HIDE); }
void window_set_caption(Window window, const char* title) { SetWindowText(window, title); }
void window_close(Window window) { CloseWindow(window); }
void window_destroy(Window window) { DestroyWindow(window); }