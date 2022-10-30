#pragma once

#include <tchar.h>
#include <windows.h>
#include <stdbool.h> 
#include "String_t.h"
#include "smart_ptr.h"

// type //

typedef struct Widget {

} Widget;

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

LRESULT CALLBACK windowCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

_Window* window_new() {
  WNDCLASSEXA wc = {};

  wc.cbSize = sizeof(WNDCLASSEXA);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = windowCallback;
  wc.hInstance = GetModuleHandle(NULL);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.lpszClassName = "simple";

  RegisterClassExA(&wc);

  _Window* window = (_Window*)malloc(sizeof(_Window));
  window->width = 800;
  window->height = 600;
  RECT wr = { 0, 0, window->width, window->height };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  window->id = CreateWindowExA(
    0,                   // extended window style
    wc.lpszClassName,    // pointer to registered class name
    wc.lpszClassName,    // pointer to window name
    WS_OVERLAPPEDWINDOW, // window style
    CW_USEDEFAULT,       // horizontal position of window
    CW_USEDEFAULT,       // vertical position of window
    wr.right - wr.left,  // window width
    wr.bottom - wr.top,  // window height
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
  while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT)
      return false;
  }
  return true;
}
void window_show(_Window* window) {
  ShowWindow(window->id, SW_SHOWDEFAULT);
}
void window_hide(_Window* window) {
  ShowWindow(window->id, SW_HIDE);
}
void window_close() {}
void window_destroy() {}
void window_fullscreen() {}
void window_set_title() {}