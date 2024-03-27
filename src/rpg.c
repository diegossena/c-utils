#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/image.h>
#include <sdk/keyboard.h>
#include <sdk/window.h>
#include <sdk/sdk.h>

#include <rpg/game.h>

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
    .flags = WINDOW_FLAG_NO_MAXIMIZE,
    .onpreload = game_onpreload,
    .onload = game_onload
  };
  window_startup(&app, &options);
  app_run(&app);
  console_log(CONSOLE_FORE_LIGHTGREEN "SUCCESS" CONSOLE_RESET);
}