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
void player_update(player_t* this, f32 elapsed) {
  // consts
  static const f32 max_velocity_y = 100.f;
  static const f32 max_velocity_x = 10.f;
  static const f32 velocity_y_per_tick = 6.f;
  const f32 velocity_x_per_tick = this->on_ground ? 25.f : 15.f;
  // handle input
  if (keyboard_pressed(KEY_UP)) {
    this->velocity_y = -velocity_y_per_tick * elapsed;
  } else if (keyboard_pressed(KEY_DOWN)) {
    this->velocity_y = velocity_y_per_tick * elapsed;
  }
  if (keyboard_pressed(KEY_LEFT)) {
    this->velocity_x -= velocity_x_per_tick * elapsed;
  } else if (keyboard_pressed(KEY_RIGHT)) {
    this->velocity_x += velocity_x_per_tick * elapsed;
  }
  if (this->on_ground && this->velocity_y == 0 && keyboard_pressed(KEY_SPACE)) {
    this->velocity_y = -12.0f;
  }
  // drag
  if (this->on_ground) {
    this->velocity_x += -3.0f * this->velocity_x * elapsed;
    if (math_abs(elapsed) < 0.01f)
      elapsed = 0.0f;
    this->velocity_y += -3.0f * this->velocity_y * elapsed;
    if (math_abs(elapsed) < 0.01f)
      elapsed = 0.0f;
  }
  // gravity
  this->velocity_y += 20.f * elapsed;
  // clamp velocities
  this->velocity_x = math_clamp(this->velocity_x, -max_velocity_x, max_velocity_x);
  this->velocity_y = math_clamp(this->velocity_y, -max_velocity_y, max_velocity_y);
  // calculate potential new position
  f32 tmp_x = this->x + this->velocity_x * elapsed;
  f32 tmp_y = this->y + this->velocity_y * elapsed;
  // collision
  if (this->velocity_x < 0) {
      // Moving Left
    if (tmp_x < 0) {
      tmp_x = 0;
      this->velocity_x = 0;
    } else if (get_tile(this->world, tmp_x, this->y) != '.' || get_tile(this->world, tmp_x, this->y + .935f) != '.') {
      tmp_x = (i32)tmp_x + 1;
      this->velocity_x = 0;
    }
  } else if (this->velocity_x > 0) {
    // Moving Right
    if (tmp_x >= this->world->level_width - 1
      || get_tile(this->world, tmp_x + 1.f, this->y) != '.'
      || get_tile(this->world, tmp_x + 1.f, this->y + .935f) != '.'
      ) {
      tmp_x = (i32)tmp_x;
      this->velocity_x = 0;
    }
  }
  this->on_ground = false;
  if (this->velocity_y < 0) {
    // Moving Up
    if (get_tile(this->world, this->x, tmp_y) != '.'
      || get_tile(this->world, this->x + .935f, tmp_y) != '.') {
      tmp_y = (i32)tmp_y + 1;
      this->velocity_y = 0;
    }
  } else if (this->velocity_y > 0) {
    // Moving Down
    if (get_tile(this->world, this->x, tmp_y + 1.f) != '.'
      || get_tile(this->world, this->x + .935f, tmp_y + 1.f) != '.') {
      tmp_y = (i32)tmp_y;
      this->velocity_y = 0;
      this->on_ground = true; // Player has a solid surface underfoot
    }
  }
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