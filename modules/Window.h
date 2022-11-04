#pragma once

#include <tchar.h>
#include <stdbool.h>

#include <windows.h>
#include <d3d11.h>

#include "smart_ptr.h"

// definitions //

typedef enum WindowFlags {
  WINDOW_HIDDEN = 0x1,
  WINDOW_RESIZABLE = 0x2,
  WINDOW_BORDERLESS = 0x4,
  WINDOW_DIRECT3D11 = 0x8,
} WindowFlags;

typedef enum WindowEvent {

  // WINDOW

  WINDOW_UNDEFINED = WM_TIMER,
  WINDOW_FOCUS = WM_SETFOCUS,
  WINDOW_BLUR = WM_KILLFOCUS,
  WINDOW_RESIZE = WM_SIZE,
  WINDOW_CLOSE = WM_CLOSE,
  WINDOW_QUIT = WM_QUIT,
  WINDOW_LOAD = WM_CREATE,

  // MOUSE

  MOUSE_MOVE = WM_MOUSEMOVE,

  MOUSE_LEFT_DOWN = WM_LBUTTONDOWN,
  MOUSE_LEFT_UP = WM_LBUTTONUP,
  MOUSE_LEFT_DBLCLK = WM_LBUTTONDBLCLK,

  MOUSE_RIGHT_DOWN = WM_RBUTTONDOWN,
  MOUSE_RIGHT_UP = WM_RBUTTONUP,
  MOUSE_RIGHT_DBLCLK = WM_RBUTTONDBLCLK,

  MOUSE_MIDDLE_DOWN = WM_MBUTTONDOWN,
  MOUSE_MIDDLE_UP = WM_MBUTTONUP,
  MOUSE_MIDDLE_DBLCLK = WM_MBUTTONDBLCLK,

  MOUSE_SCROLL = WM_MOUSEWHEEL,

  // Keyboard

  WINDOW_KEY_DOWN = WM_KEYDOWN,
  WINDOW_KEY_UP = WM_KEYUP
} WindowEvent;

// type



typedef void WindowCallback(Window* window);

typedef struct Window {
  HWND id;
  WindowEvent event;
  WPARAM wParam;
  LPARAM lParam;
  WindowCallback* callback;
  Direct3D11 gfx;
} Window;

typedef struct Direct3D11 {
  IDXGISwapChain* swapchain;
  ID3D11Device* dev;
  ID3D11DeviceContext* devcon;
  ID3D11RenderTargetView* backbuffer;

  ID3D11InputLayout* layout;        // the pointer to the input layout
  ID3D11Buffer* vertexBuffer;       // the pointer to the vertex buffer
  ID3D11Buffer* indexBuffer;        // the pointer to the index buffer

  D3D11_BUFFER_DESC bufferDesc;
} Direct3D11;

// handlers //

LRESULT CALLBACK wndProc(
  HWND hWnd,
  UINT message,
  WPARAM wParam,
  LPARAM lParam
) {
  Window* window = 0;
  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    window = (Window*)pcs->lpCreateParams;
    SetWindowLongPtrA(
      hWnd,
      GWLP_USERDATA,
      (LONG_PTR)window
    );
  } else {
    window = (Window*)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
  }
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_NCHITTEST:
    case WM_SETCURSOR:
    case WM_CONTEXTMENU:
      break;
    case WM_CLOSE:
      free(window);
      DestroyWindow(hWnd);
    default: {
      window->event = message;
      window->lParam = lParam;
      window->wParam = wParam;
    }
  }
  if (window && window->callback)
    (*window->callback)(window);
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
    .style = CS_DBLCLKS
  };

  Window* window = (Window*)malloc(sizeof(Window));

  // d3d11 //
  window->gfx.bufferDesc.ByteWidth = 0;
  window->gfx.bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  window->gfx.bufferDesc.BindFlags = 0;
  window->gfx.bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  window->gfx.bufferDesc.MiscFlags = 0;
  window->gfx.bufferDesc.StructureByteStride = 0;

  DXGI_SWAP_CHAIN_DESC scd = {
    .BufferCount = 1, // one back buffer,
    .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // use 32-bit color,
    .BufferDesc.Width = width, // set the back buffer width
    .BufferDesc.Height = height, // set the back buffer height
    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT, // how swap chain is to be used
    .OutputWindow = window->id, // the window to be used
    .SampleDesc.Count = 4, // how many multisamples
    .Windowed = TRUE, // windowed/full-screen mode
    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH // allow full-screen switching
  };
  // create a device, device context and swap chain using the information in the scd struct
  D3D11CreateDeviceAndSwapChain(
    0,
    D3D_DRIVER_TYPE_HARDWARE,
    0,
    0,
    0,
    0,
    D3D11_SDK_VERSION,
    &scd,
    &window->gfx.swapchain,
    &window->gfx.dev,
    0,
    &window->gfx.devcon
  );
  // get the address of the back buffer
  ID3D11Texture2D* pBackBuffer;
  GetBuffer(0, __uuidof(), (LPVOID*)&pBackBuffer);
  CreateRenderTargetView(pBackBuffer, 0, &window->gfx.backbuffer);
   // InitGraphics


   //

  RegisterClassEx(&wc);
  window->id = CreateWindowEx(
    0,                // extended window style
    wc.lpszClassName, // pointer to registered class name
    wc.lpszClassName, // pointer to window name
    dwStyle,          // window style
    x,                // horizontal position of window
    y,                // vertical position of window
    width,            // window width
    height,           // window height
    0,                // handle to parent or owner window
    0,                // handle to menu, or child-window identifier
    wc.hInstance,     // handle to application instance
    window // pointer to window-creation data
  );
  SetTimer(window->id, 0, USER_TIMER_MINIMUM, NULL);
}
void window_new_centered(
  WindowCallback callback,
  const char* title,
  unsigned int width, unsigned int height,
  WindowFlags flags) {
  window_new(
    callback, title,
    (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2),
    width, height,
    flags);
}
void window_new_fullscreen(WindowCallback callback, const char* title, WindowFlags flags) {
  window_new(
    callback, title, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
    flags | WINDOW_BORDERLESS);
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
void window_set_renderer(Window* window) {}

// Screen //

int screen_get_width() { return GetSystemMetrics(SM_CXSCREEN); }
int screen_get_height() { return GetSystemMetrics(SM_CYSCREEN); }

// Keyboard //

typedef enum Keycode {
  KEY_BACK = 0x8,
  KEY_TAB,
  KEY_ENTER = 0xd,
  KEY_SHIFT = 0x10,
  KEY_CTRL,
  KEY_ALT,
  KEY_PAUSE,
  KEY_CAPS,
  KEY_ESC = 0x1b,
  KEY_SPACE = 0x20,
  KEY_LEFT = 0x25,
  KEY_UP,
  KEY_RIGHT,
  KEY_DOWN,
} Keycode;

WORD window_keycode(Window* window) { return window->wParam; }