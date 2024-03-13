#include <sdk/window.h>

#include "time.h"
#include "world.h"
#include "player.h"

void oncreate(window_t* this) {
  time_start();
  world_start();
  player_start();
}
void onresize(window_t* this) {
  world_on_resize(this);
}
void onupdate(window_t* this) {
  time_update();
  player_update();
  world_set_camera(player_x, player_y);
  world_render(this);
  player_render(this);
}

i32 main() {
  application_t app;
  app_constructor(&app);
  window_options_t options = {
    .name = "Game Test",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .oncreate = oncreate,
    .onupdate = onupdate,
    .onresize = onresize
  };
  window_startup(&app, &options);
  return app_run(&app);
}

// utils