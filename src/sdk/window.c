#include <sdk/window.h>

bool global_window_running = false;
bool global_window_focus = false;
u8 global_window_keyboard_count = 0;
u8 global_window_keyboard_state[32] = {};

bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return global_window_keyboard_state[byte_index] & (1 << bit_index);
}

export bool window_focused() {
  return global_window_focus;
}