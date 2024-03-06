#include "internal/platform.h"

#if PLATFORM_WINDOWS

// window_pooling
#include "internal/window.h"
// app_global
#include "internal/application.h"
#include <windows.h>
// TODO: remove this line
#include "sdk/console.h"

void window_pooling() {
  MSG msg;
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      --app_global.tasks_count;
      return;
    }
  }
}

#endif