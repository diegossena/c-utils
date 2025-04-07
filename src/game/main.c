#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>
#include <sdk/window.h>
#include <sdk/date.h>
#include <sdk/unity.h>

u64 render_time = 0;
f32 position [] = { 0, 0 };

void window_onkeyup() {}
void window_onkeydown(key_code_t key) {}

void window_onkeypress() {
  if (window_key_pressed(KEY_UP)) {
    position[1] -= 1;
  } else if (window_key_pressed(KEY_DOWN)) {
    position[1] += 1;
  }
  if (window_key_pressed(KEY_LEFT)) {
    position[0] -= 1;
  } else if (window_key_pressed(KEY_RIGHT)) {
    position[0] += 1;
  }
  window_redraw();
}

void window_onrender() {
  u64 time = date_now();
  console_log("window_onrender %lu", time - render_time);
  render_time = time;
  f32 size [] = { 100, 100 };
  window_clear();
  window_fill_rectangle(
    position[0], position[1],
    position[0] + size[0], position[1] + size[1],
    1, 0, 0, 1
  );
}

i32 main(i32 argc, char** argv) {
  window_startup("Game", 800, 600);
  sync_wait(global_window_onload_sync);
  u64 time = date_now();
  while (global_window_thread) {
    u64 now = date_now();
    u64 delta_time = now - time;
    if (delta_time > 15) {
      time = now;
      position[0] += 1;
      window_redraw();
    }
  }
  console_log("global_window_thread %d", global_window_thread);
  return 0;
}