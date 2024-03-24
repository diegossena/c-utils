#include "./game/game.h"

i32 main() {
  console_startup();
  application_t app;
  app_new(&app);
  window_props_t options = {
    .name = "RPG",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .flags = WINDOW_FLAG_NO_RESIZABLE | WINDOW_FLAG_NO_MAXIMIZE,
    .onpreload = game_onpreload,
    .onload = game_onload
  };
  window_startup(&app, &options);
  app_run(&app);
  console_log_cstr(CONSOLE_FORE_GREEN "SUCCESS" CONSOLE_RESET);
}

// utils