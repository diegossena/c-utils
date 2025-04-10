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
  const f32 tile_size = 1.f / (6.f / 160.f);
  // vertices
  vertex_t vertex;
  vertex.x = -.5f;
  vertex.y = -.5f;
  vertex.u = 0.f;
  vertex.v = 1.f;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = -.5f;
  vertex.y = .5f;
  vertex.u = 0.f;
  vertex.v = 0.f;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = .5f;
  vertex.y = .5f;
  vertex.u = 1.f;
  vertex.v = 0.f;
  global_vertices_virtual[global_vertices_length++] = vertex;
  vertex.x = .5f;
  vertex.y = -.5f;
  vertex.u = 1.f;
  vertex.v = 1.f;
  global_vertices_virtual[global_vertices_length++] = vertex;
  // indexes
  global_indexes_virtual[global_indexes_length++] = 0;
  global_indexes_virtual[global_indexes_length++] = 1;
  global_indexes_virtual[global_indexes_length++] = 2;
  global_indexes_virtual[global_indexes_length++] = 0;
  global_indexes_virtual[global_indexes_length++] = 2;
  global_indexes_virtual[global_indexes_length++] = 3;

}