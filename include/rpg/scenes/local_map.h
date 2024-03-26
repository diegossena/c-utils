#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>
#include <sdk/math.h>

#include <rpg/game.h>
#include <rpg/components/player.h>

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 76

typedef struct local_map_t {
  game_t* game;
  // event_listener_t
  event_listener_t ondraw;
  event_listener_t onkeypress;
  event_listener_t onkeydown;
  event_listener_t onkeyup;
  event_listener_t destroy;
  // layers
  byte bg0[TILEMAP_SIZE];
  byte bg1[TILEMAP_SIZE];
  byte bg2[TILEMAP_SIZE];
  // tilemap
  u8 visible_tiles_x, visible_tiles_y;
  vector2d_t offset;
  u8 tile_offset_x, tile_offset_y;
  vector2d_t offset_limit;
  vector2d_t camera;
  // data
  u8 health;
  // components
  player_t player;
} local_map_t;

#include <rpg/components/player.inl.h>
#include <rpg/maps/introduction.h>

void localmap_onresize(local_map_t* this) {
  window_t* window = this->game->window;
  f32 visible_tiles_x = (f32)window->width / TILE_SIZE;
  f32 visible_tiles_y = (f32)window->height / TILE_SIZE;
  this->visible_tiles_x = math_ceil(visible_tiles_x);
  this->visible_tiles_y = math_ceil(visible_tiles_y);
  this->offset_limit.x = (f32)TILEMAP_WIDTH - visible_tiles_x;
  this->offset_limit.y = (f32)TILEMAP_WIDTH - visible_tiles_y;
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
  // draw
  gfx_image_t tile = {
    .window = window,
    .src = &game->terrain_atlas,
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
        case '.': // Grass
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
void localmap_draw(local_map_t* this) {
  game_t* game = this->game;
  window_t* window = game->window;
  // calculate top-leftmost visible tile
  this->offset.x = this->camera.x - (f32)this->visible_tiles_x / 2.f;
  this->offset.y = this->camera.y - (f32)this->visible_tiles_y / 2.f;
  // clamp camera to game boudaries
  this->offset.x = math_clamp(this->offset.x, 0.f, this->offset_limit.x);
  this->offset.y = math_clamp(this->offset.y, 0.f, this->offset_limit.y);
  // Get offsets for smooth movement
  this->tile_offset_x = (this->offset.x - (i32)this->offset.x) * TILE_SIZE;
  this->tile_offset_y = (this->offset.y - (i32)this->offset.y) * TILE_SIZE;
  // render
  tilemap_draw(this, this->bg0);
  tilemap_draw(this, this->bg1);
  player_draw(&this->player);
  tilemap_draw(this, this->bg2);
}
void localmap_onkeypress(local_map_t* this) {
  player_onkeypress(&this->player);
}
void localmap_onkeydown(local_map_t* this) {
  player_onkeydown(&this->player);
}
void localmap_onkeyup(local_map_t* this) {
  player_onkeyup(&this->player);
}
void localmap_destroy(local_map_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeypress);
  emitter_off(&this->onkeydown);
  emitter_off(&this->onkeyup);
  emitter_off(&this->destroy);
  memory_free(this);
}
void scene_localmap_load(game_t* game) {
  assert(game);
  // init
  local_map_t* this = memory_alloc0(sizeof(local_map_t));
  window_t* window = game->window;
  this->game = game;
  this->health = 10;
  localmap_onresize(this);
  tilemap_introduction_load(this);
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
  this->onkeypress = (event_listener_t) {
    .callback = (listener_t)localmap_onkeypress,
    .context = this
  };
  emitter_on(&window->onkeypress, &this->onkeypress);
  this->onkeydown = (event_listener_t) {
    .callback = (listener_t)localmap_onkeydown,
    .context = this
  };
  emitter_on(&window->onkeydown, &this->onkeydown);
  this->onkeyup = (event_listener_t) {
    .callback = (listener_t)localmap_onkeyup,
    .context = this
  };
  emitter_on(&window->onkeyup, &this->onkeyup);
  // player
  this->player.map = this;
  player_new(&this->player);
}