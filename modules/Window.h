#pragma once

#include <tchar.h>
#include <windows.h>
#include <stdbool.h> 
#include "String_t.h"
#include "smart_ptr.h"

// definitions //

#define WINDOWPOS_CENTERED_MASK    0x2FFF0000u
#define WINDOWPOS_CENTERED_DISPLAY(X)  (WINDOWPOS_CENTERED_MASK|(X))
#define WINDOWPOS_CENTERED         WINDOWPOS_CENTERED_DISPLAY(0)

typedef enum WindowFlags {
  WINDOW_SHOWN = 1,
  WINDOW_RESIZABLE,
  WINDOW_FULLSCREEN
} WindowFlags;

// WS_OVERLAPPED
// WS_CAPTION
// WS_SYSMENU
// WS_MINIMIZEBOX

// type //

typedef struct Window {
  HWND id;
  unsigned int width;
  unsigned int height;
} _Window;

// memory control //

void _window_free(const _Window* window) {
  free(*(void**)window);
}
#define Window smart _Window

// handlers //

LRESULT CALLBACK windowCallback(
  HWND hWnd,
  UINT message,
  WPARAM wParam,
  LPARAM lParam
) {
  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    _Window* window = (_Window*)pcs->lpCreateParams;
    SetWindowLongPtrA(
      hWnd,
      GWLP_USERDATA,
      (LONG_PTR)window
    );
    return 1;
  }
  _Window* window = (_Window*)GetWindowLongPtrA(hWnd, GWLP_USERDATA);

  if (window) {
    switch (message) {
    case WM_SETFOCUS:
      // window->hasFocus_ = true;
      break;
    case WM_KILLFOCUS:
      // window->hasFocus_ = false;
      break;
    case WM_SIZE: {
      window->width = LOWORD(lParam);
      window->height = HIWORD(lParam);
      //window->onResize(width, height);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      return 1;
    }
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

// constructors //

_Window* window_new(
  const char* title,
  unsigned int x, unsigned int y,
  unsigned int width, unsigned int height,
  WindowFlags flags
) {
  size_t dwExStyle = 0;
  size_t dwStyle = 0;
  if (flags & WINDOW_FULLSCREEN) {

  }
  else {
    dwStyle = WS_OVERLAPPED
      | WS_CAPTION
      | WS_SYSMENU
      | WS_MINIMIZEBOX;
    if (flags & WINDOW_SHOWN)
      dwStyle |= WS_VISIBLE;
    if (flags & WINDOW_RESIZABLE)
      dwStyle |= WS_THICKFRAME;
    if (x == WINDOWPOS_CENTERED)
      x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
    if (y == WINDOWPOS_CENTERED)
      y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
  }

  WNDCLASSEX wc = {
    .cbSize = sizeof(WNDCLASSEX),
    .lpfnWndProc = windowCallback,
    .lpszClassName = title,
    .hCursor = LoadCursor(0, IDC_ARROW),
  };
  RegisterClassEx(&wc);
  _Window* window = (_Window*)malloc(sizeof(_Window));
  window->width = width;
  window->height = height;
  window->id = CreateWindowEx(
    dwExStyle,           // extended window style
    wc.lpszClassName,    // pointer to registered class name
    wc.lpszClassName,    // pointer to window name
    dwStyle,             // window style
    x,                   // horizontal position of window
    y,                   // vertical position of window
    window->width,       // window width
    window->height,      // window height
    0,                   // handle to parent or owner window
    0,                   // handle to menu, or child-window identifier
    wc.hInstance,        // handle to application instance
    window               // pointer to window-creation data
  );
  return window;
}

// methods //

bool window_run() {
  MSG msg;
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT)
      return false;
  }
  return true;
}
void window_show(_Window* window) { ShowWindow(window->id, SW_SHOWDEFAULT); }
void window_hide(_Window* window) { ShowWindow(window->id, SW_HIDE); }
void window_set_caption(_Window* window, const char* title) { SetWindowText(window->id, title); }
void window_close(_Window* window) { CloseWindow(window->id); }
void window_destroy(_Window* window) { DestroyWindow(window->id); }