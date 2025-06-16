#include <game/titlescreen.scene.h>
#include <game/tilemap.h>
#include <game/game.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#define TITLESCREEN_VERTICES_USED 84
#define TITLESCREEN_INDEXES_USED 126

bool titlescreen = false;

void titlescreen_load() {
  titlescreen = true;
  // background
  window_background[0] = 120.f / 255.f;
  window_background[1] = 168.f / 255.f;
  window_background[2] = 192.f / 255.f;
  vertices_reserve(
    vertices_capacity + TITLESCREEN_VERTICES_USED,
    indexes_capacity + TITLESCREEN_INDEXES_USED
  );
}
void titlescreen_unload() {
  titlescreen = false;
  vertices_reserve(
    vertices_capacity - TITLESCREEN_VERTICES_USED,
    indexes_capacity - TITLESCREEN_INDEXES_USED
  );
}
void titlescreen_onkeydown(key_t key) {
  switch (key) {
    case KEY_SPACE:
      transition_load(titlescreen_unload, tilemap_load);
      break;
    default:
      return;
  }
}
void titlescreen_render() {
  text_draw("GAME", -.5f, .9f, 120.f);
  f32 x0 = 0;
  f32 y0 = -.25f;
  f32 size = 50.f;
  text_draw("PRESS SPACE", x0, y0, size);
  x0 += .12f;
  y0 -= size * window_ndc_y;
  text_draw("TO START", x0, y0, size);
}