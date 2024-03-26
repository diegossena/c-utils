#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/keyboard.h>

#include <rpg/components/player.h>
#include <rpg/scenes/local_map.h>

void player_onupdate(player_t* this) {
  if (this->walking_timer <= 0)
    return;
  static const f32 duration = .25f;
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  this->walking_timer += window->elapsed_time;
  f32 progress = this->walking_timer / duration;
  if (progress > 1.f) {
    progress = 1.f;
  } else if (this->start_state == this->state && progress >= .075f) {
    this->state = (++this->state) % PLAYER_STATE_MAX;
  }
  this->x = this->start_x + progress * this->distance_x;
  this->y = this->start_y + progress * this->distance_y;
  local_map->camera.x = this->x + .5f;
  local_map->camera.y = this->y + 1.f;
  if (progress >= 1.f) {
    this->distance_x = 0;
    this->distance_y = 0;
    this->walking_timer = 0;
    this->state = (++this->state) % PLAYER_STATE_MAX;
  }
  window_render_request(window);
}
void player_draw(player_t* this) {
  static const f32 sprite_width = 15.f;
  static const f32 sprite_height = 31.f;
  static const f32 scale = 4.f;
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  gfx_image_t player = {
    .window = game->window,
    .src = &game->character_png,
    .src_width = sprite_width,
    .src_height = sprite_height,
    .extend_mode = BITMAP_EXTEND_COVER,
    .rect = {
       (this->x - local_map->offset.x) * TILE_SIZE + 1.f,
      (this->y - local_map->offset.y) * TILE_SIZE + 1.f,
    },
  };
  switch (this->direction) {
    case PLAYER_UP:
      player.src_position.y = 75;
      break;
    case PLAYER_DOWN:
      player.src_position.y = 42;
      break;
    case PLAYER_RIGHT:
      player.src_position.y = 141;
      break;
    case PLAYER_LEFT:
      player.src_position.y = 108;
      break;
  }
  switch (this->state) {
    case PLAYER_STATE_STANDING_1:
    case PLAYER_STATE_STANDING_2:
      player.src_position.x = 25;
      break;
    case PLAYER_STATE_WALKING_1:
      player.src_position.x = 8;
      break;
    case PLAYER_STATE_WALKING_2:
      player.src_position.x = 42;
      break;
  }
  rect_set_width(&player.rect, sprite_width * scale);
  rect_set_height(&player.rect, sprite_height * scale);
  gfx_image_draw(&player);
}
void player_onkeypress(player_t* this) {
  if (this->walking_timer > 0)
    return;
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  this->walking_timer += window->elapsed_time;
  this->start_x = this->x;
  this->start_y = this->y;
  this->start_state = this->state;
  bool update = false;
  if (keyboard_pressed(KEY_UP)) {
    this->distance_y -= 1.f;
    this->direction = PLAYER_UP;
    update = true;
  } else if (keyboard_pressed(KEY_DOWN)) {
    this->distance_y += 1.f;
    this->direction = PLAYER_DOWN;
    update = true;
  }
  if (keyboard_pressed(KEY_RIGHT)) {
    this->distance_x += 1.f;
    this->direction = PLAYER_RIGHT;
    update = true;
  } else if (keyboard_pressed(KEY_LEFT)) {
    this->distance_x -= 1.f;
    this->direction = PLAYER_LEFT;
    update = true;
  }
  if (update) {
    this->state = (++this->state) % PLAYER_STATE_MAX;
    window_render_request(window);
  }
}
void player_onkeydown(player_t* this) {
}
void player_onkeyup(player_t* this) {
  // local_map_t* local_map = this->map;
  // game_t* game = local_map->game;
  // window_t* window = game->window;
  // if (this->state % 2) {
  //   this->state = (++this->state) % PLAYER_STATE_MAX;
  //   this->walking_animation = 0;
  //   window_render_request(window);
  // }
}
void player_new(player_t* this) {
  assert(this->map);
  this->direction = PLAYER_DOWN;
  this->state = PLAYER_STATE_STANDING_1;
  this->walking_timer = 0;
}
void player_free(player_t* this) {

}