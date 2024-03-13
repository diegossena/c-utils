#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/keyboard.h>

bool keyboard_pressed(key_code_t key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

#endif