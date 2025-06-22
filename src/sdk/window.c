#include <sdk/window.h>

bool window_resized;
f32 window_deltatime;
f32 window_ndc_x;
f32 window_ndc_y;
bool window_focus;
u8 _keyboard_count;
u8 _keyboard_state[32];
u64 vertices_capacity;
u64 indexes_capacity;
u64 _vertices_length;
u64 _indexes_length;
u16 mouse_x;
u16 mouse_y;
vertex_t* _vertices_virtual;
u32* _indexes_virtual;
f64 _render_time;

const f32 atlas_ndc_x = 1.f / ATLAS_WIDTH;
const f32 atlas_ndc_y = 1.f / ATLAS_HEIGHT;

bool window_key_pressed(key_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return _keyboard_state[byte_index] & (1 << bit_index);
}

void window_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 u0, f32 v0, f32 u1, f32 v1
) {
  // vertex
  u32 vertex_offset = _vertices_length;
  vertex_t vertex = { 0 };
  vertex.x = x0;
  vertex.y = y0;
  vertex.texcoord[0] = u0;
  vertex.texcoord[1] = v0;
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y0;
  vertex.texcoord[0] = u1;
  vertex.texcoord[1] = v0;
  vertices_push(vertex);
  vertex.x = x1;
  vertex.y = y1;
  vertex.texcoord[0] = u1;
  vertex.texcoord[1] = v1;
  vertices_push(vertex);
  vertex.x = x0;
  vertex.y = y1;
  vertex.texcoord[0] = u0;
  vertex.texcoord[1] = v1;
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
  u32 vertex_offset = _vertices_length;
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