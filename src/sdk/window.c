#include <sdk/window.h>
#include <sdk/buffer.h>

bool global_window_repaint = true;
bool global_window_resize = true;

f32 global_gfx_deltatime = 0;

u16 global_window_width = 800;
u16 global_window_height = 600;
bool global_window_focus = false;
u8 global_window_keyboard_count = 0;
u8 global_window_keyboard_state[32] = {};
thread_t* global_window_thread = 0;

vertex_t* global_vertices;
u32* global_indices;

export bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return global_window_keyboard_state[byte_index] & (1 << bit_index);
}