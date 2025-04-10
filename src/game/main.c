#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

void window_onkeyup() {}
void window_onkeydown(key_code_t key) {
  if (key == KEY_ESCAPE) {
    return window_close();
  }
  if (titlescreen.loaded) {
    titlescreen_onkeydown(key);
  }
  if (tilemap.loaded) {
    tilemap_onkeydown(key);
  }
  if (tilemap.loaded) {
    tilemap_onkeydown(key);
  }
}
void window_onkeypress() {}
void window_onrender() {
  if (titlescreen.loaded) {
    titlescreen_render();
  }
  if (tilemap.loaded) {
    tilemap_render();
  }
  if (transition.loading) {
    transition_render();
  }
}

i32 main(i32 argc, char** argv) {
  window_startup();
  window_set_title("Game");
  window_atlas_load("assets/atlas.bin", 160, 160);
  titlescreen_load();
  window_run();
  if (titlescreen.loaded) {
    titlescreen_unload();
  }
  return 0;
}