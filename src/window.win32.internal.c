#include "internal/platform.h"

#if PLATFORM_WINDOWS
// app_global
#include "internal/application.h"
#include <windows.h>

void window_pooling() {
  MSG msg;
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      app_global.window_pooling = false;
      return;
    }
  }
}

#endif