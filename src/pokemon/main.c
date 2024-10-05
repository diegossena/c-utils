
#include <sdk/date.h>

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
void window_onupdate(u32 time) {}
void window_onkeyup() {}
void window_onkeydown() {}
void window_onkeypress() {
  f32 velocity = 10.f;
  if (window_key_pressed(KEY_DOWN)) {
    top += velocity;
    bottom += velocity;
    window_redraw();
  } else if (window_key_pressed(KEY_UP)) {
    top -= velocity;
    bottom -= velocity;
    window_redraw();
  }
  if (window_key_pressed(KEY_LEFT)) {
    left -= velocity;
    right -= velocity;
    window_redraw();
  } else if (window_key_pressed(KEY_RIGHT)) {
    left += velocity;
    right += velocity;
    window_redraw();
  }
}

// 304374 bytes
i32 main(i32 argc, char** argv) {
  window_startup("Test", 800, 600);
  window_run();
  console_log("SUCCESS");
  return 0;
}