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
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      // All painting occurs here, between BeginPaint and EndPaint.

      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

      EndPaint(hWnd, &ps);
    }
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
  size_t dwStyle = flags & WINDOW_BORDERLESS
    ? WS_POPUP
    : WS_SYSMENU | WS_MINIMIZEBOX;
  if (!(flags & WINDOW_HIDDEN))
    dwStyle |= WS_VISIBLE;
  if (flags & WINDOW_RESIZABLE)
    dwStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
  WNDCLASSEX wc = {
    .cbSize = sizeof(WNDCLASSEX),
    .lpfnWndProc = windowCallback,
    .lpszClassName = title,
    .hCursor = LoadCursor(0, IDC_ARROW),
  };
  RegisterClassEx(&wc);
  _Window* window = (_Window*)malloc(sizeof(_Window));
  window->id = CreateWindowEx(
    0, // extended window style
    wc.lpszClassName, // pointer to registered class name
    wc.lpszClassName, // pointer to window name
    dwStyle, // window style
    x, // horizontal position of window
    y, // vertical position of window
    window->width = width, // window width
    window->height = height, // window height
    0, // handle to parent or owner window
    0, // handle to menu, or child-window identifier
    wc.hInstance, // handle to application instance
    window // pointer to window-creation data
  );
  return window;
}
_Window* window_new_centered(
  const char* title,
  unsigned int width, unsigned int height,
  WindowFlags flags
) {
  return window_new(
    title,
    (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2),
    width, height,
    flags
  );
}
_Window* window_new_fullscreen(const char* title, WindowFlags flags) {
  return window_new(
    title, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
    WINDOW_BORDERLESS | flags
  );
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