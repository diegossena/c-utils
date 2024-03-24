#include <sdk/platform.h>

#ifdef PLATFORM_WINDOWS

#include <sdk/keyboard.h>
#include <windows.h>

bool keyboard_pressed(key_code_t key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

#endif