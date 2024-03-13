#pragma once

#include <sdk/window.h>
#include <sdk/keyboard/keyboard.h>

#include "world.h"

typedef struct player_t {
  f32 x, y;
  f32 velocity_x, velocity_y;
  bool on_ground;
  world_t* world;
} player_t;

void player_start(player_t* this, world_t* world) {
  this->x = 0;
  this->y = 0;
  this->velocity_x = 0;
  this->velocity_y = 0;
  this->on_ground = true;
  this->world = world;
}
void player_update(player_t* this, f32 delta_time) {
  // consts
  f32 velocity_y = 6.f;
  f32 velocity_x = this->on_ground ? 25.f : 15.f;
  f32 max_velocity_y = 100.f;
  f32 max_velocity_x = 10.f;
  // handle input
  if (keyboard_pressed(KEY_UP)) {
    this->velocity_y = -velocity_y;
  } else if (keyboard_pressed(KEY_DOWN)) {
    this->velocity_y = velocity_y;
  }
  if (keyboard_pressed(KEY_LEFT)) {
    this->velocity_x = -velocity_x;
  } else if (keyboard_pressed(KEY_RIGHT)) {
    this->velocity_x = velocity_x;
  }
  if (keyboard_pressed(KEY_SPACE)) {
    if (this->velocity_x == 0) {
      this->velocity_x = -12.0f;
    }
  }
  // drag
  if (this->on_ground) {
    this->velocity_x += -3.0f * this->velocity_x * delta_time;
    if (math_abs(delta_time) < 0.01f)
      delta_time = 0.0f;
    this->velocity_y += -3.0f * this->velocity_y * delta_time;
    if (math_abs(delta_time) < 0.01f)
      delta_time = 0.0f;
  }
  // gravity
  // this->velocity_y += 20.f * delta_time;
  // clamp velocities
  this->velocity_x = math_clamp(this->velocity_x, -this->velocity_x, this->velocity_x);
  this->velocity_y = math_clamp(this->velocity_y, -this->velocity_y, this->velocity_y);
  // calculate potential new position
  f32 tmp_x = this->x + this->velocity_x * delta_time;
  f32 tmp_y = this->y + this->velocity_y * delta_time;
  // TODO: collision
  // apply new position
  this->x = tmp_x;
  this->y = tmp_y;
}

void player_render(player_t* this) {
  rect_props_t player_props = {
    .rect = {
      .left = (this->x - this->world->offset_x) * world_tile_size,
      .top = (this->y - this->world->offset_y) * world_tile_size,
    },
    .color = { 1.f, 0.f, 0.f, 1.f }
  };
  rect_set_size(&player_props.rect, world_tile_size);
  gfx_draw_rect(this->world->window, &player_props);
}