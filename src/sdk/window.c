#include <sdk/window.h>

bool __global_window_running = false;
bool __global_window_focus = false;
u8 __global_window_keyboard_count = 0;
u8 __global_window_keyboard_state[32] = {};

bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return __global_window_keyboard_state[byte_index] & (1 << bit_index);
}
void __window_key_press(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  __global_window_keyboard_state[byte_index] |= (1 << bit_index);
}
void __window_key_release(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  __global_window_keyboard_state[byte_index] &= ~(1 << bit_index);
}

SDK_EXPORT bool window_focused() {
  return __global_window_focus;
}