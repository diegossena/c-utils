#pragma once

#include <sdk/types.h>
#include <sdk/assert.h>
#include <sdk/window.h>
#include <sdk/math/geometry_2d.h>
#include <sdk/sdk.h>
#include <rpg/game.h>

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 76

typedef struct tilemap_t {
  game_t* game;
  // listeners
  event_listener_t onresize;
  event_listener_t ondestroy;
  // layers
  u8 bg0[TILEMAP_SIZE];
  u8 bg1[TILEMAP_SIZE];
  u8 bg2[TILEMAP_SIZE];
  // camera
  u8 visible_tiles_x, visible_tiles_y;
  vector2d_t offset;
  u8 tile_offset_x, tile_offset_y;
  vector2d_t offset_limit;
} tilemap_t;

char tilemap_tiles_get(const u8* tiles, i32 x, i32 y) {
  if (x >= 0 && x < TILEMAP_WIDTH && y >= 0 && y < TILEMAP_WIDTH) {
    return tiles[y * TILEMAP_WIDTH + x];
  }
  return -1;
}

void tilemap_onresize(tilemap_t* this) {
  window_t* window = this->game->window;
  f32 visible_tiles_x = (f32)window->width / TILE_SIZE;
  f32 visible_tiles_y = (f32)window->height / TILE_SIZE;
  this->visible_tiles_x = math_ceil(visible_tiles_x);
  this->visible_tiles_y = math_ceil(visible_tiles_y);
  this->offset_limit.x = (f32)TILEMAP_WIDTH - visible_tiles_x;
  this->offset_limit.y = (f32)TILEMAP_WIDTH - visible_tiles_y;
}
void tilemap_camera_update(tilemap_t* this, f32 x, f32 y) {
  x += .75f;
  y += .5f;
  // calculate top-leftmost visible tile
  this->offset.x = x - (f32)this->visible_tiles_x / 2.f;
  this->offset.y = y - (f32)this->visible_tiles_y / 2.f;
  // clamp camera to game boudaries
  this->offset.x = math_clamp(this->offset.x, 0.f, this->offset_limit.x);
  this->offset.y = math_clamp(this->offset.y, 0.f, this->offset_limit.y);
  // Get offsets for smooth movement
  this->tile_offset_x = (this->offset.x - (i32)this->offset.x) * TILE_SIZE;
  this->tile_offset_y = (this->offset.y - (i32)this->offset.y) * TILE_SIZE;
}
void tilemap_draw(tilemap_t* this, const u8* layer) {
  game_t* game = this->game;
  window_t* window = game->window;
  // draw
  gfx_image_t tile = {
    .window = window,
    .src = &game->pallet_town_interiors,
    .extend_mode = BITMAP_EXTEND_COVER
  };
  for (i32 x = -1; x < this->visible_tiles_x + 1; x++) {
    for (i32 y = -1; y < this->visible_tiles_y + 1; y++) {
      char tile_id = tilemap_tiles_get(layer, x + this->offset.x, y + this->offset.y);
      switch (tile_id) {
        case 'T': // Tree
          tile.src_position.x = 417;
          tile.src_position.y = 989;
          tile.src_width = 30;
          tile.src_height = 32;
          break;
        case 'B': // Bush
          tile.src_position.x = 202;
          tile.src_position.y = 840;
          tile.src_width = 77;
          tile.src_height = 77;
          break;
        case ' ': // Grass
          tile.src_position.x = 0;
          tile.src_position.y = 800;
          tile.src_width = 32;
          tile.src_height = 32;
          break;
        case 'q': // grass-roof (0, 0)
          tile.src_position.x = 224;
          tile.src_position.y = 768;
          tile.src_width = 31;
          tile.src_height = 31;
          break;
        case 'w': // grass-roof (1, 0)
          tile.src_position.x = 256;
          tile.src_position.y = 768;
          tile.src_width = 31;
          tile.src_height = 31;
          break;
        case 'a': // grass-roof (1, 0)
          tile.src_position.x = 224;
          tile.src_position.y = 800;
          tile.src_width = 31;
          tile.src_height = 31;
          break;
        case 's': // grass-roof (1, 1)
          tile.src_position.x = 256;
          tile.src_position.y = 800;
          tile.src_width = 31;
          tile.src_height = 31;
          break;
        default:
          continue;
      }
      tile.rect.left_top.x = x * TILE_SIZE - this->tile_offset_x;
      tile.rect.left_top.y = y * TILE_SIZE - this->tile_offset_y;
      rect_set_size(&tile.rect, TILE_SIZE);
      gfx_image_draw(&tile);
    }
  }
}
void tilemap_free(tilemap_t* this) {
  emitter_off(&this->ondestroy);
  emitter_off(&this->onresize);
}
void tilemap_new(tilemap_t* this, game_t* game) {
  this->game = game;
  window_t* window = this->game->window;
  tilemap_onresize(this);
  // register
  this->onresize = (event_listener_t) {
    .callback = (listener_t)tilemap_onresize,
    .context = this
  };
  emitter_on(&window->onresize, &this->onresize);
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)tilemap_free,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
}