#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>
#include <game/game.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

titlescreen_t titlescreen;
#define TITLESCREEN_VERTICES_USED 84
#define TITLESCREEN_INDEXES_USED 126


export void titlescreen_load() {
  titlescreen.loaded = true;
  window_updated = true;
  // background
  window_background[0] = 120.f / 255.f;
  window_background[1] = 168.f / 255.f;
  window_background[2] = 192.f / 255.f;
  vertices_reserve(vertices_capacity + TITLESCREEN_VERTICES_USED);
  indexes_reserve(indexes_capacity + TITLESCREEN_INDEXES_USED);
  window_updated = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  vertices_reserve(vertices_capacity - TITLESCREEN_VERTICES_USED);
  indexes_reserve(indexes_capacity - TITLESCREEN_INDEXES_USED);
}
export void titlescreen_onkeydown(key_t key) {
  switch (key) {
    case KEY_SPACE:
      transition_load(titlescreen_unload, hero_home_2f_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  text_draw("GAME", 120.f, -.5f, 1.f);
  text_draw("PRESS SPACE", 90.f, .1f, -.5f);
  text_draw("TO START", 90.f, .22f, -.66f);
}