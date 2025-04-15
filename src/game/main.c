#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

void window_onkeyup() {}
void window_onresize() {
  if (tilemap.loaded) {
    tilemap_onresize();
  }
}
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
// 512332 bytes
i32 main(i32 argc, char** argv) {
  window_startup("assets/atlas.bin");
  window_set_title("Game");
  titlescreen_load();
  window_run();
  return 0;
}