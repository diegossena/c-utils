#include <sdk/window.h>

bool window_has_update = true;
bool window_resized = true;

f32 window_deltatime = 0;

u16 window_width = 800;
u16 window_height = 600;
f32 ndc_per_px_x;
f32 ndc_per_px_y;
bool window_focus = false;
u8 keyboard_count = 0;
u8 keyboard_state[32] = {};
thread_t* window_thread = 0;

vertex_t* vertices_virtual;
u64 vertices_length;
u64 vertices_capacity;
u64 vertices_used;

u32* indexes_virtual;
u64 indexes_length;
u64 indexes_capacity;
u64 indexes_used;

f32 window_background[4] = { 0, 0, 0, 1 };

export bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return keyboard_state[byte_index] & (1 << bit_index);
}