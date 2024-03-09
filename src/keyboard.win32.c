#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/window/keyboard.h"
#include <windows.h>

bool is_key_pressed(key_code key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

#endif