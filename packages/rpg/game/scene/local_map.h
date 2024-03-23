#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>
#include <sdk/math.h>

#include "../game.h"

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 60

typedef struct local_map_t {
  game_t* game;
  // event_listener_t
  event_listener_t onupdate;
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // camera
  u8 visible_tiles_x, visible_tiles_y;
  f32 offset_limit_x, offset_limit_y;
  vector2d_t camera;
  vector2d_t screen_padding;
  // data
  byte bg0[TILEMAP_SIZE];
  byte bg1[TILEMAP_SIZE];
  byte bg2[TILEMAP_SIZE];
  // elements
  showdialog_t hp_display;
} local_map_t;

#include "../maps/introduction.h"

// void localmap_load(game_t*);
// void localmap_destroy(local_map_t*);
void localmap_onresize(local_map_t* this) {
  window_t* window = this->game->window;
  f32 visible_tiles_x = (f32)window->width / TILE_SIZE;
  f32 visible_tiles_y = (f32)window->height / TILE_SIZE;
  this->visible_tiles_x = math_ceil(visible_tiles_x);
  this->visible_tiles_y = math_ceil(visible_tiles_y);
  this->offset_limit_x = (f32)TILEMAP_WIDTH - visible_tiles_x;
  this->offset_limit_y = (f32)TILEMAP_WIDTH - visible_tiles_y;
}
char tilemap_tiles_get(const byte* tiles, i32 x, i32 y) {
  if (x >= 0 && x < TILEMAP_WIDTH && y >= 0 && y < TILEMAP_WIDTH) {
    return tiles[y * TILEMAP_WIDTH + x];
  }
  return '\0';
}
void tilemap_draw(local_map_t* this, const byte* layer) {
  game_t* game = this->game;
  window_t* window = game->window;
  // calculate top-leftmost visible tile
  this->camera.x = math_clamp(this->camera.x, 0.f, this->offset_limit_x);
  this->camera.y = math_clamp(this->camera.y, 0.f, this->offset_limit_y);
  vector2d_t offset = {
    this->camera.x - (f32)this->visible_tiles_x / 2.f,
    this->camera.y - (f32)this->visible_tiles_y / 2.f
  };
  // clamp camera to game boudaries
  offset.x = math_clamp(offset.x, 0.f, this->offset_limit_x);
  offset.y = math_clamp(offset.y, 0.f, this->offset_limit_y);
  // Get offsets for smooth movement
  f32 tile_offset_x = (offset.x - (i32)offset.x) * TILE_SIZE;
  f32 tile_offset_y = (offset.y - (i32)offset.y) * TILE_SIZE;
  // draw
  for (i32 x = -1; x < this->visible_tiles_x + 1; x++) {
    for (i32 y = -1; y < this->visible_tiles_y + 1; y++) {
      char tile_id = tilemap_tiles_get(layer, x + offset.x, y + offset.y);
      gfx_image_t tile = {
        .window = window,
        .rect = {
          x * TILE_SIZE - math_round(tile_offset_x),
          y * TILE_SIZE - math_round(tile_offset_y),
          .width = TILE_SIZE, .height = TILE_SIZE
        },
        .src = &game->terrain_atlas,
        .extend_mode = BITMAP_EXTEND_COVER,
      };
      rect_update_size(&tile.rect);
      switch (tile_id) {
        case 'T': // Tree
          tile.position.x = 417;
          tile.position.y = 989;
          tile.size.width = 30;
          tile.size.height = 32;
          break;
        case 'B':
          tile.position.x = 202;
          tile.position.y = 840;
          tile.size.width = 77;
          tile.size.height = 77;
          break;
        case '.':
          tile.position.x = 2;
          tile.position.y = 802;
          tile.size.width = 2;
          tile.size.height = 2;
          break;
      }
      gfx_image_draw(&tile);
    }
  }
}
void localmap_draw(local_map_t* this) {
  tilemap_draw(this, this->bg0);
  tilemap_draw(this, this->bg1);
  showdialog_draw(&this->hp_display);
}
void localmap_onupdate(local_map_t* this) {
  f32 velocity = 4.f * this->game->window->elapsed_time;
  if (keyboard_pressed(KEY_UP)) {
    this->camera.y -= velocity;
    window_render(this->game->window);
  } else if (keyboard_pressed(KEY_DOWN)) {
    this->camera.y += velocity;
    window_render(this->game->window);
  }
  if (keyboard_pressed(KEY_RIGHT)) {
    this->camera.x += velocity;
    window_render(this->game->window);
  } else if (keyboard_pressed(KEY_LEFT)) {
    this->camera.x -= velocity;
    window_render(this->game->window);
  }
}
void localmap_onkeydown(local_map_t* this) {

}
void localmap_destroy(local_map_t* this) {
  emitter_off(&this->onupdate);
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  showdialog_free(&this->hp_display);
  memory_free(this);
}
void localmap_load(game_t* game) {
  // init
  local_map_t* this = memory_alloc(sizeof(local_map_t));
  window_t* window = game->window;
  this->game = game;
  localmap_onresize(this);
  tilemap_introduction_load(this);
  // register
  this->onupdate = (event_listener_t) {
    .callback = (listener_t)localmap_onupdate,
    .context = this
  };
  emitter_on(&window->onupdate, &this->onupdate);
  this->ondraw = (event_listener_t) {
    .callback = (listener_t)localmap_draw,
    .context = this
  };
  emitter_on(&window->ondraw, &this->ondraw);
  this->destroy = (event_listener_t) {
    .callback = (listener_t)localmap_destroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->destroy);
  this->onkeydown = (event_listener_t) {
    .callback = (listener_t)localmap_onkeydown,
    .context = this
  };
  emitter_on(&window->onkeydown, &this->onkeydown);

  // elements

  // hp_display
  this->hp_display.game = game;
  showdialog_new(&this->hp_display);
  wstring_append_cwstr(&this->hp_display.text, L"HP: 10/10");
  this->hp_display.position.x = 10.f;
  this->hp_display.position.y = 10.f;
  showdialog_update(&this->hp_display);
}