#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>

#include "../game.h"

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 16

typedef struct local_map_t {
  game_t* game;
  // event_listener_t
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // data
  vector2d_t camera;
  u8 visible_tiles_x, visible_tiles_y;
  byte back[TILEMAP_SIZE];
  byte fore[TILEMAP_SIZE];
  // elements
  showdialog_t hp_display;
} local_map_t;

#include "../maps/introduction.h"

// void localmap_load(game_t*);
// void localmap_destroy(local_map_t*);

char tilemap_tiles_get(const byte* tiles, i32 x, i32 y) {
  if (x >= 0 && x < TILEMAP_WIDTH && y >= 0 && y < TILEMAP_WIDTH) {
    return tiles[TILEMAP_WIDTH * TILEMAP_WIDTH + y * TILEMAP_WIDTH + x];
  }
  return '\0';
}
void tilemap_draw(local_map_t* this, u8 layer) {
  window_t* window = this->game->window;
  const byte* tiles = layer ? this->back : this->fore;
  // calculate top-leftmost visible tile
  vector2d_t offset = {
    this->camera.x - (f32)this->visible_tiles_x / 2.f,
    this->camera.y - (f32)this->visible_tiles_y / 2.f
  };
  for (i32 x = -1; x < TILEMAP_WIDTH + 1; x++) {
    for (i32 y = -1; y < TILEMAP_WIDTH + 1; y++) {
      char tile_id = tilemap_tiles_get(tiles, x + offset.x, y + offset.y);
      // gfx_rect_t tile_props = {
      //   .rect = {
      //     x * TILE_SIZE - tile_world_offset_x,
      //     y * TILE_SIZE - tile_world_offset_y,
      //   },
      //   .color = {.a = 1.f }
      // };
      // switch (tile_id) {
      //   case '.': // Sky
      //     tile_props.color.r = 0.f;
      //     tile_props.color.g = 1.f;
      //     tile_props.color.b = 1.f;
      //     break;
      //   case '#': // Solid Block
      //     tile_props.color.r = 1.f;
      //     break;
      // }
    }
  }
}
void localmap_draw(local_map_t* this) {
  showdialog_draw(&this->hp_display);
}
void localmap_onkeydown(local_map_t* this) {

}
void localmap_onresize(local_map_t* this) {
  window_t* window = this->game->window;
  this->visible_tiles_x = window->width / TILE_SIZE;
  this->visible_tiles_y = window->height / TILE_SIZE;
}
void localmap_destroy(local_map_t* this) {
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
  // register
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

  localmap_onresize(this);
  tilemap_introduction_load(this);
}