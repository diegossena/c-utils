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
}
void window_onkeypress() {}
void window_onrender() {
  window_clear();
  if (titlescreen.loaded) {
    titlescreen_render();
  }
  if (transition.loading) {
    transition_render();
  }
}

i32 main(i32 argc, char** argv) {
  // window
  window_startup();
  window_set_title("Game");
  // load
  const wchar_t* paths [] = {
    L"assets/megaman_2.ttf"
  };
  gfx_font_load(paths, sizeof(paths) / sizeof(wchar_t*));
  titlescreen_load();
  // loop
  f64 time = time_now_f64();
  const f64 frame_rate = 1. / 60;
  while (global_window_thread) {
    f64 now = time_now_f64();
    global_delta_time = now - time;
    if (global_delta_time > frame_rate) {
      time = now;
      if (global_repaint) {
        window_redraw();
        if (!transition.loading) {
          global_repaint = false;
        }
      }
    }
  }
  // close
  if (titlescreen.loaded) {
    titlescreen_unload();
  }
  window_shutdown();
  return 0;
}