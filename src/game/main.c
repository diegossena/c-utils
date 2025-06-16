#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

void window_onresize() {
  if (global_tilemap) {
    tilemap_onresize();
  }
}
void window_onkeydown(key_t key) {
  if (key == KEY_ESCAPE) {
    return window_close();
  }
  if (titlescreen) {
    titlescreen_onkeydown(key);
  }
}
void window_onkeypress() {
  if (global_tilemap) {
    tilemap_onkeypress();
  }
}
void window_onrender() {
  if (titlescreen) {
    titlescreen_render();
  }
  if (global_tilemap) {
    tilemap_draw();
  }
  if (transition.loading) {
    transition_render();
  }
}
void window_dblclick() {}
void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {}
void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_onkeyup() {}
void window_onscroll(i32 delta) {}
void window_onmousemove() {}
// 512332 bytes
i32 main(i32 argc, char** argv) {
  window_startup("Game", "share/atlas.bin");
  titlescreen_load();
  window_run();
  return 0;
}