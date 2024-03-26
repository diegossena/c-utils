#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/keyboard.h>

#include <rpg/components/player.h>
#include <rpg/scenes/local_map.h>

void player_update(player_t* this) {}
void player_draw(player_t* this) {
  static const f32 sprite_width = 15.f;
  static const f32 sprite_height = 31.f;
  static const f32 scale = 4.f;
  local_map_t* map = this->map;
  game_t* game = map->game;
  gfx_image_t player = {
    .window = game->window,
    .src = &game->character_png,
    .src_width = sprite_width,
    .src_height = sprite_height,
    .extend_mode = BITMAP_EXTEND_COVER,
    .rect = {
      (this->x - map->offset.x) * TILE_SIZE,
      (this->y - map->offset.y) * TILE_SIZE,
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
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  static const f32 velocity = 5.f;
  bool update = false;
  if (keyboard_pressed(KEY_UP)) {
    this->direction = PLAYER_UP;
    this->y -= velocity * window->elapsed_time;
    update = true;
  } else if (keyboard_pressed(KEY_DOWN)) {
    this->direction = PLAYER_DOWN;
    this->y += velocity * window->elapsed_time;
    update = true;
  }
  if (keyboard_pressed(KEY_RIGHT)) {
    this->direction = PLAYER_RIGHT;
    this->x += velocity * window->elapsed_time;
    update = true;
  } else if (keyboard_pressed(KEY_LEFT)) {
    this->direction = PLAYER_LEFT;
    this->x -= velocity * window->elapsed_time;
    update = true;
  }
  if (update) {
    this->x = math_clamp(this->x, 0.f, TILEMAP_WIDTH - 1);
    this->y = math_clamp(this->y, 0.f, TILEMAP_WIDTH - 1);
    local_map->camera.x = this->x;
    local_map->camera.y = this->y;
    this->walking_animation += window->elapsed_time;
    if (this->walking_animation >= .15f) {
      this->state = (++this->state) % PLAYER_STATE_MAX;
      this->walking_animation = 0;
    }
    window_render_request(window);
  }
}
void player_onkeydown(player_t* this) {}
void player_onkeyup(player_t* this) {
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  if (this->state != PLAYER_STATE_STANDING_1) {
    this->state = PLAYER_STATE_STANDING_1;
    this->walking_animation = 0;
    window_render_request(window);
  }
}
void player_new(player_t* this) {
  assert(this->map);
  this->direction = PLAYER_DOWN;
  this->state = PLAYER_STATE_STANDING_1;
  this->walking_animation = 0;
}
void player_free(player_t* this) {

}