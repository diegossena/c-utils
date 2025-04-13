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

u16 atlas_width;
u16 atlas_height;

export bool window_key_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return keyboard_state[byte_index] & (1 << bit_index);
}

extern void window_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 u0, f32 v0, f32 u1, f32 v1
) {
  // vertex
  u32 vertex_offset = vertices_length;
  vertex_t vertex = {};
  vertex.x = x0;
  vertex.y = y0;
  vertex.uv[0] = u0;
  vertex.uv[1] = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertex.uv[0] = u1;
  vertex.uv[1] = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertex.uv[0] = u1;
  vertex.uv[1] = v1;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertex.uv[0] = u0;
  vertex.uv[1] = v1;
  vertices_virtual[vertices_length++] = vertex;
  // indexes
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 1;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset + 3;
}
extern void window_rect_fill(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 r, f32 g, f32 b, f32 a
) {
  // vertex
  u32 vertex_offset = vertices_length;
  vertex_t vertex = {
    .color = { r, g, b, a }
  };
  vertex.x = x0;
  vertex.y = y0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertices_virtual[vertices_length++] = vertex;
  // indexes
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 1;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset + 3;
}