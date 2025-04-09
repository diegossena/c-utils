#include <sdk/window.h>

// events
bool global_window_repaint = true;
bool global_window_resize = true;
// control
f32 global_gfx_deltatime = 0;
// render
u16 global_window_width = 800;
u16 global_window_height = 600;
bool global_window_focus = false;
u8 global_window_keyboard_count = 0;
u8 global_window_keyboard_state[32] = {};
thread_t* global_window_thread = 0;

export bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return global_window_keyboard_state[byte_index] & (1 << bit_index);
}