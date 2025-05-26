#include <sdk/window.h>
// window
bool window_updated = true;
f32 window_deltatime = 0;
f32 window_pixel_ndc[2];
bool window_focus = false;
f32 window_background[4] = { 0, 0, 0, 1 };
// window_mouse
i32 mouse_x = 0;
i32 mouse_y = 0;
// keyboard
u8 keyboard_count = 0;
u8 keyboard_state[32] = { 0 };
// atlas
const u16 atlas_tiles_width = atlas_width / atlas_tile_size;
const u16 atlas_tiles_height = atlas_height / atlas_tile_size;
// gfx
vertex_t* vertices_virtual;
u64 vertices_length;
u64 vertices_capacity = 0;

u32* indexes_virtual;
u64 indexes_length;
u64 indexes_capacity = 0;

bool window_key_pressed(key_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return keyboard_state[byte_index] & (1 << bit_index);
}

void window_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 u0, f32 v0, f32 u1, f32 v1
) {
  // vertex
  u32 vertex_offset = vertices_length;
  vertex_t vertex = { 0 };
  vertex.x = x0;
  vertex.y = y0;
  vertex.textcoord[0] = u0;
  vertex.textcoord[1] = v0;
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y0;
  vertex.textcoord[0] = u1;
  vertex.textcoord[1] = v0;
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y1;
  vertex.textcoord[0] = u1;
  vertex.textcoord[1] = v1;
  vertices_push(vertex);
  vertex.x = x0;
  vertex.y = y1;
  vertex.textcoord[0] = u0;
  vertex.textcoord[1] = v1;
  vertices_push(vertex);
  // indexes
  indexes_push(vertex_offset);
  indexes_push(vertex_offset + 1);
  indexes_push(vertex_offset + 2);
  indexes_push(vertex_offset);
  indexes_push(vertex_offset + 2);
  indexes_push(vertex_offset + 3);
}
void window_rect_fill(
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
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y0;
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y1;
  vertices_push(vertex);
  vertex.x = x0;
  vertex.y = y1;
  vertices_push(vertex);
  // indexes
  indexes_push(vertex_offset);
  indexes_push(vertex_offset + 1);
  indexes_push(vertex_offset + 2);
  indexes_push(vertex_offset);
  indexes_push(vertex_offset + 2);
  indexes_push(vertex_offset + 3);
}