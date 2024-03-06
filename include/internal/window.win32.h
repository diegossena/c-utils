#pragma once

#include "sdk/window.h"
#include "internal/task.h"
#include <windows.h>

typedef struct window_t {
  HWND handle;
} window_t;

void window_pooling();