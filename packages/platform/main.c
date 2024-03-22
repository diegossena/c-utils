#include <sdk/window.h>
#include <sdk/time.h>

#include "world.h"
#include "player.h"

f64 last_update;
world_t world;
player_t player;

void onload(window_t* this) {
  world_start(&world, this);
  player_start(&player, &world);
  last_update = time_absolute();
}
void onresize(window_t* this) {
  world_on_resize(&world);
}
void onupdate(window_t* this) {
  // timer
  f64 now = time_absolute();
  f32 elapsed = now - last_update;
  last_update = now;
  // game update
  player_update(&player, elapsed);
  world.camera_x = player.x;
  world.camera_y = player.y;
  world_render(&world);
  player_render(&player);
}

i32 main() {
  application_t app;
  app_new(&app);
  window_props_t options = {
    .name = "Platform",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .onload = onload,
    .onupdate = onupdate,
    .onresize = onresize
  };
  window_startup(&app, &options);
  return app_run(&app);
}

// utils