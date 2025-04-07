#include <sdk/window.h>

sync_t* global_window_onload_sync = 0;
bool global_window_focus = false;
u8 global_window_keyboard_count = 0;
u8 global_window_keyboard_state[32] = {};
thread_t* global_window_thread = 0;

export void window_startup(
  const char* title,
  i32 width, i32 height
) {
  assert(global_window_thread == 0);
  global_window_onload_sync = sync_new();
  global_window_thread = thread_new(__window_thread, 0);
}
export bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return global_window_keyboard_state[byte_index] & (1 << bit_index);
}