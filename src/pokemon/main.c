
#include <sdk/window.h>
#include <sdk/console.h>
#include <sdk/keyboard.h>
#include <sdk/unity.h>

f32 left = 0.f, top = 0.f, right = 100.f, bottom = 100.f;

void window_onrender() {
  window_fill_rectangle(
    left, top, right, bottom,
    1.f, 0.f, 0.f, 1.f
  );
}
void window_onupdate(u32 time) {
  f32 velocity = 10.f;
  if (keyboard_pressed(KEY_DOWN)) {
    top += velocity;
    bottom += velocity;
    window_redraw();
  } else if (keyboard_pressed(KEY_UP)) {
    top -= velocity;
    bottom -= velocity;
    window_redraw();
  }
  if (keyboard_pressed(KEY_LEFT)) {
    left -= velocity;
    right -= velocity;
    window_redraw();
  } else if (keyboard_pressed(KEY_RIGHT)) {
    left += velocity;
    right += velocity;
    window_redraw();
  }
}

// 374248 bytes
i32 main(i32 argc, char** argv) {
  window_startup("Test", 800, 600);
  window_run();
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  return 0;
}