#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>
#include <sdk/window.h>
#include <sdk/date.h>
#include <game/unity.h>

void window_onkeyup() {}
void window_onkeydown(key_code_t key) {
  if (key == KEY_ESCAPE) {
    return window_close();
  }
  if (titlescreen.loaded) {
    titlescreen_onkeydown(key);
  }
}
void window_onkeypress() {}
void window_onrender() {
  window_clear();
  if (titlescreen.loaded) {
    titlescreen_render();
  }
}

i32 main(i32 argc, char** argv) {
  // window.create
  window_startup();
  sync_wait(global_window_onload_sync);
  window_set_title("Game");
  // load
  const wchar_t* paths [] = {
    L"assets/megaman_2.ttf"
  };
  gfx_font_load(paths, sizeof(paths) / sizeof(wchar_t*));
  titlescreen_load();
  u64 time = time_now();
  while (global_window_thread) {
    u64 now = time_now();
    u64 delta_time = now - time;
    if (delta_time > 15) {
      time = now;
      if (global_repaint) {
        window_redraw();
        global_repaint = false;
      }
    }
  }
  // close
  if (titlescreen.loaded) {
    titlescreen_unload();
  }
  return 0;
}