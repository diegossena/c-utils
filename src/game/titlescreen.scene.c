#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>
#include <sdk/window.h>

titlescreen_t titlescreen;
const u32 titlescreen_vertices_used = (
  4 * 11 // "PRESS SPACE"
  + 4 * 7 // "TO PLAY"
  );

export void titlescreen_load() {
  global_vertices_used += titlescreen_vertices_used;
  titlescreen.loaded = true;
  global_window_repaint = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  global_vertices_used -= titlescreen_vertices_used;
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      // transition_load(titlescreen_unload, hero_home_2f_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  // texture
  const u8 tile_x = 0;
  const u8 tile_y = 0;
  const f32 tile_size = (1.f / 160.f) * 16.f;
  const f32 u0 = tile_size * tile_x;
  const f32 v0 = tile_size * tile_y;
  const f32 u1 = u0 + tile_size;
  const f32 v1 = v0 + tile_size;
  // vertices
  const f32 px = 0.f;
  const f32 py = 0.f;
  const f32 size = 64.f;
  const f32 ndc_w = 2.f / (f32)global_window_width;
  const f32 ndc_h = 2.f / (f32)global_window_height;

  const f32 x0 = -1.f + px * ndc_w;
  const f32 y0 = 1.f + py * ndc_h;
  const f32 x1 = x0 + ndc_w * size;
  const f32 y1 = y0 - ndc_h * size;

  vertex_t vertex;
  vertex.x = x0;
  vertex.y = y0;
  vertex.u = u0;
  vertex.v = v0;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertex.u = u1;
  vertex.v = v0;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertex.u = u1;
  vertex.v = v1;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertex.u = u0;
  vertex.v = v1;
  global_vertices_virtual[global_vertices_length++] = vertex;
  // indexes
  global_indexes_virtual[global_indexes_length++] = 0;
  global_indexes_virtual[global_indexes_length++] = 1;
  global_indexes_virtual[global_indexes_length++] = 2;
  global_indexes_virtual[global_indexes_length++] = 0;
  global_indexes_virtual[global_indexes_length++] = 2;
  global_indexes_virtual[global_indexes_length++] = 3;

}