#pragma once

#include <rpg/components/player.h>
#include <rpg/scenes/local_map.h>

void __player_onupdate(player_t* this) {
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  this->timer += window->elapsed_time;
  f32 progress = this->timer / this->duration;
  if (progress > 1.f) {
    progress = 1.f;
  }
  this->x = this->start_x + progress * this->distance_x;
  this->y = this->start_y + progress * this->distance_y;
  local_map->camera.x = this->x + .5f;
  local_map->camera.y = this->y + 1.f;
  if (progress >= 1.f) {
    this->state = (++this->state) % PLAYER_STATE_MAX;
    emitter_off(&this->onupdate);
    emitter_on(&window->onkeypress, &this->onkeypress);
  }
  window_render_request(window);
}
void __player_onkeypress(player_t* this) {
  local_map_t* local_map = this->map;
  game_t* game = local_map->game;
  window_t* window = game->window;
  this->start_x = this->x;
  this->start_y = this->y;
  this->start_state = this->state;
  this->distance_x = 0;
  this->distance_y = 0;
  this->timer = 0;
  bool update = false;
  static const f32 walk_duration = .225f;
  static const f32 flip_duration = .1f;
  if (keyboard_pressed(KEY_DOWN)) {
    if (this->direction == PLAYER_DOWN) {
      this->distance_y += 1.f;
      this->duration = walk_duration;
    } else {
      this->direction = PLAYER_DOWN;
      this->duration = flip_duration;
    }
    update = true;
  } else if (keyboard_pressed(KEY_UP)) {
    if (this->direction == PLAYER_UP) {
      this->distance_y -= 1.f;
      this->duration = walk_duration;
    } else {
      this->direction = PLAYER_UP;
      this->duration = flip_duration;
    }
    update = true;
  } else  if (keyboard_pressed(KEY_LEFT)) {
    if (this->direction == PLAYER_LEFT) {
      this->distance_x -= 1.f;
      this->duration = walk_duration;
    } else {
      this->direction = PLAYER_LEFT;
      this->duration = flip_duration;
    }
    update = true;
  } else  if (keyboard_pressed(KEY_RIGHT)) {
    if (this->direction == PLAYER_RIGHT) {
      this->distance_x += 1.f;
      this->duration = walk_duration;
    } else {
      this->direction = PLAYER_RIGHT;
      this->duration = flip_duration;
    }
    update = true;
  }
  if (update) {
    this->state = (++this->state) % PLAYER_STATE_MAX;
    emitter_off(&this->onkeypress);
    emitter_on(&window->onupdate, &this->onupdate);
    window_render_request(window);
  }
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
      (this->x - local_map->offset.x) * TILE_SIZE + 7.f,
      (this->y - local_map->offset.y) * TILE_SIZE + 26.f,
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
void player_free(player_t* this) {
  emitter_off(&this->onupdate);
  emitter_off(&this->onkeypress);
  emitter_off(&this->onupdate);
}
void player_new(player_t* this) {
  assert(this->map);
  window_t* window = this->map->game->window;
  this->direction = PLAYER_DOWN;
  this->state = PLAYER_STATE_STANDING_1;
  this->onupdate = (event_listener_t) {
    .callback = (listener_t)__player_onupdate,
    .context = this
  };
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)player_free,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
  this->onkeypress = (event_listener_t) {
    .callback = (listener_t)__player_onkeypress,
    .context = this
  };
  emitter_on(&window->onkeypress, &this->onkeypress);
}