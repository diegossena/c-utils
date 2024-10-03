#include <sdk/keyboard.h>
#ifdef PLATFORM_WINDOWS

#include <winuser.h>
bool keyboard_pressed(key_code_t key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

#endif