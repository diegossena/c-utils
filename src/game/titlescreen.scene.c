#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>
#include <sdk/window.h>

titlescreen_t titlescreen;
const u32 titlescreen_vertices_used = (
  4 * 10 // "PRESS SPACE"
  + 4 * 6 // "TO PLAY"
  );

export void titlescreen_load() {
  vertices_used += titlescreen_vertices_used;
  titlescreen.loaded = true;
  window_has_update = true;
  // background
  window_background[0] = 120.f / 255.f;
  window_background[1] = 168.f / 255.f;
  window_background[2] = 192.f / 255.f;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  vertices_used -= titlescreen_vertices_used;
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
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
  text_draw("PRESS SPACE", 90.f, .1f, -.5f);
  text_draw("TO START", 90.f, .22f, -.66f);
}