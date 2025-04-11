#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>
#include <sdk/window.h>

titlescreen_t titlescreen;
const u32 titlescreen_vertices_used = (
  4 * 11 // "PRESS SPACE"
  + 4 * 7 // "TO PLAY"
  );
const f32 tile_screen_size = 60.f;

export void titlescreen_load() {
  vertices_used += titlescreen_vertices_used;
  titlescreen.loaded = true;
  window_has_update = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  vertices_used -= titlescreen_vertices_used;
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
void tile_render(f32 x0, f32 y0, u8 tile_x, u8 tile_y) {
  // tile
  const f32 image_ngc_per_px = (1.f / 160.f) * 16.f;
  const f32 u0 = image_ngc_per_px * tile_x;
  const f32 v0 = image_ngc_per_px * tile_y;
  const f32 u1 = u0 + image_ngc_per_px;
  const f32 v1 = v0 + image_ngc_per_px;
  // vertex
  const f32 x1 = x0 + ndc_per_px_x * tile_screen_size;
  const f32 y1 = y0 - ndc_per_px_y * tile_screen_size;
  u32 vertex_offset = vertices_length;
  vertex_t vertex;
  vertex.x = x0;
  vertex.y = y0;
  vertex.u = u0;
  vertex.v = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertex.u = u1;
  vertex.v = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertex.u = u1;
  vertex.v = v1;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertex.u = u0;
  vertex.v = v1;
  vertices_virtual[vertices_length++] = vertex;
  // indexes
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 1;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset + 3;
}
export void titlescreen_render() {
  // PRESS SPACE
  f32 x = .5f;
  f32 px_space = 35.f;
  tile_render(x, 0.f, 3, 6);
  x += px_space * ndc_per_px_x;
  tile_render(x, 0.f, 5, 6);
  // x += px_space * ndc_per_px_x;
  // tile_render(x, 0.f, 2, 6);
}