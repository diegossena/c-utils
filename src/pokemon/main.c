
#include <sdk/window.h>
#include <sdk/unity.h>

i32 main(i32 argc, char** argv) {
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);
  window_t window;
  window_constructor(&window, "Test", 800, 600);
  taskmanager_run(&taskmanager, &window);
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  return 0;
}