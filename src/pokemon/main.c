
#include <sdk/window.h>
#include <sdk/console.h>
#include <sdk/unity.h>

void window_onupdate(u32 time) {
  console_log("window_onupdate %lu", time);
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