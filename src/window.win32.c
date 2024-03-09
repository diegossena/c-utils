#include "internal/platform.h"

#if PLATFORM_WINDOWS

// windows.h, d3d11.h, sdk/window/window.h
#include "internal/window.win32.h"
#include "internal/memory.h"
#include "internal/application.h"
#include "sdk/error.h"
#include "sdk/console.h" // TODO: remove this lines

void window_inicialize(window_opt* options) {
  LPCSTR class_name = "MainWClass";
  if (FindWindowA(class_name, null)) {
    error("FindWindowA", ERR_UNKNOWN);
    return;
  }
  window_t* this = memory_alloc0(sizeof(window_t));
  // RegisterWindowClass
  WNDCLASSEXA wc = {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    .lpfnWndProc = window_procedure,
    .hInstance = GetModuleHandleA(null),
    .hCursor = LoadCursor(null, IDC_ARROW),
    .lpszClassName = class_name,
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
  // Register Events
  this->onupdate = options->onupdate;
  this->onmousemove = options->onmousemove;
  this->onmousedown = options->onmousedown;
  this->onmouseup = options->onmouseup;
  this->onkeydown = options->onkeydown;
  this->onkeyup = options->onkeyup;
  this->ondblclick = options->ondblclick;
  this->onresize = options->onresize;
  this->oncreate = options->oncreate;
  this->onclose = options->onclose;
    // CreateWindow
  CreateWindowExA(
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
  return;
clear:
  memory_free(this);
}

u16 window_get_screen_width(window_t* this) {
  RECT rect;
  GetWindowRect(this->handle, &rect);
  return rect.right - rect.left;
}
u16 window_get_screen_height(window_t* this) {
  RECT rect;
  GetWindowRect(this->handle, &rect);
  return rect.bottom - rect.top;
}

#endif