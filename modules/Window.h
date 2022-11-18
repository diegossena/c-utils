#pragma once

// definitions //

#define COBJMACROS
typedef struct Window Window;

// headers //

#include <tchar.h>
#include <stdbool.h>

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include "smart_ptr.h"

// libraries //

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")

// type //

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

typedef void WindowCallback(Window* window);
typedef struct Window {
  HWND id;
  WindowEvent event;
  WPARAM wParam;
  LPARAM lParam;
  WindowCallback* callback;

  // Direct3D 11

  IDXGISwapChain* swapchain;
  ID3D11Device* dev;
  ID3D11DeviceContext* devcon;
  ID3D11RenderTargetView* rtView;

  ID3D11InputLayout* layout;        // the pointer to the input layout
  ID3D11Buffer* vertexBuffer;       // the pointer to the vertex buffer
  ID3D11Buffer* indexBuffer;        // the pointer to the index buffer

  D3D11_BUFFER_DESC bufferDesc;
} Window;

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

  if (message == WM_CLOSE) {
    printf("WM_CLOSE %x %d\n", message, window);
  }

  switch (message) {
    case WM_DESTROY:
      KillTimer(hWnd, 0);
      free(window);
      PostQuitMessage(0);
      window = 0;
      break;

    case WM_CLOSE:
      window = 0;
      DestroyWindow(hWnd);
      break;

    case WM_NCDESTROY:
    case WM_NCHITTEST:
    case WM_SETCURSOR:
    case WM_CONTEXTMENU:
      window = 0;
      break;
  }
  if (window) {
    window->event = message;
    window->lParam = lParam;
    window->wParam = wParam;
    (*window->callback)(window);
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
    .style = CS_DBLCLKS
  };

  Window* window = (Window*)malloc(sizeof(Window));

  // d3d11 //
  window->bufferDesc.ByteWidth = 0;
  window->bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  window->bufferDesc.BindFlags = 0;
  window->bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  window->bufferDesc.MiscFlags = 0;
  window->bufferDesc.StructureByteStride = 0;

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
    &window->swapchain,
    &window->dev,
    0,
    &window->devcon
  );

  // get the address of the back buffer
  ID3D11Texture2D* backBuffer_p;
  IDXGISwapChain_GetBuffer(window->swapchain, 0, &IID_ID3D11Texture2D, (void**)&backBuffer_p);

  // use the back buffer address to create the render target
  ID3D11Device_CreateRenderTargetView(window->dev, (ID3D11Resource*)backBuffer_p, 0, &window->rtView);
  ID3D11Texture2D_Release(backBuffer_p);

  // set the render target as the back buffer
  ID3D11DeviceContext_OMSetRenderTargets(window->devcon, 1, &window->rtView, 0);

  // Set the viewport
  D3D11_VIEWPORT viewport = {
      .TopLeftX = 0,
      .TopLeftY = 0,
      .Width = (FLOAT)width,
      .Height = (FLOAT)height,
      .MinDepth = 0,
      .MaxDepth = 1,
  };
  ID3D11DeviceContext_RSSetViewports(window->devcon, 1, &viewport);

  // select which primtive type we are using
  ID3D11DeviceContext_IASetPrimitiveTopology(window->devcon, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // window register //

  RegisterClassEx(&wc);
  window->callback = window_callback;
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

Keycode window_keycode(Window* window) { return window->wParam; }