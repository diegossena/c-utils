#pragma once

#include <sdk/sdk.h>

#include "world.h"
#include "time.h"

f32 player_x, player_y;
f32 player_velocity_x, player_velocity_y;
bool player_on_ground;

void player_start() {
  player_x = 0;
  player_y = 0;
  player_velocity_x = 0;
  player_velocity_y = 0;
  player_on_ground = true;
}
void player_update() {
  // consts
  f32 velocity_y = 6.f;
  f32 velocity_x = player_on_ground ? 25.f : 15.f;
  f32 max_velocity_y = 100.f;
  f32 max_velocity_x = 10.f;
  // handle input
  if (keyboard_pressed(KEY_UP)) {
    player_velocity_y = -velocity_y;
  } else if (keyboard_pressed(KEY_DOWN)) {
    player_velocity_y = velocity_y;
  }
  if (keyboard_pressed(KEY_LEFT)) {
    player_velocity_x = -velocity_x;
  } else if (keyboard_pressed(KEY_RIGHT)) {
    player_velocity_x = velocity_x;
  }
  if (keyboard_pressed(KEY_SPACE)) {
    if (player_velocity_x == 0) {
      player_velocity_x = -12.0f;
    }
  }
  // drag
  if (player_on_ground) {
    player_velocity_x += -3.0f * player_velocity_x * delta_time;
    if (math_abs(delta_time) < 0.01f)
      delta_time = 0.0f;
    player_velocity_y += -3.0f * player_velocity_y * delta_time;
    if (math_abs(delta_time) < 0.01f)
      delta_time = 0.0f;
  }
  // gravity
  // player_velocity_y += 20.f * delta_time;
  // clamp velocities
  player_velocity_x = math_clamp(player_velocity_x, -player_velocity_x, player_velocity_x);
  player_velocity_y = math_clamp(player_velocity_y, -player_velocity_y, player_velocity_y);
  // calculate potential new position
  f32 tmp_x = player_x + player_velocity_x * delta_time;
  f32 tmp_y = player_y + player_velocity_y * delta_time;
  // TODO: collision
  // apply new position
  player_x = tmp_x;
  player_y = tmp_y;
}

void player_render(window_t* window) {
  rect_props_t player_props = {
    .rect = {
      .left = (player_x - world_offset_x) * tile_size,
      .top = (player_y - world_offset_y) * tile_size,
    },
    .color = { 1.f, 0.f, 0.f, 1.f }
  };
  rect_set_size(&player_props.rect, tile_size);
  gfx_draw_rect(window, &player_props);
}