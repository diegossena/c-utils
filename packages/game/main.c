#include <sdk/window.h>
#include <sdk/time/time.h>

#include "world.h"
#include "player.h"

f32 last_update;
world_t world;
player_t player;

u64 frames = 0;
f32 frame_time = 0;

void oncreate(window_t* this) {
  world_start(&world, this);
  player_start(&player, &world);
  last_update = time_absolute();
}
void onresize(window_t* this) {
  world_on_resize(&world);
}
void onupdate(window_t* this) {
  f32 now = time_absolute();
  f32 delta_time = now - last_update;
  last_update = now;
  player_update(&player, delta_time);
  world.camera_x = player.x;
  world.camera_y = player.y;
  world_render(&world);
  player_render(&player);
  ++frames;
  frame_time += delta_time;
  if (frame_time >= 1.f) {
    console_log("frames=%d", frames);
    frame_time = 0;
  }
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