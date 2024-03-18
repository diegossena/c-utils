#pragma once

#include <sdk/window.h>

const i32 world_tile_size = 32;
typedef struct world_t {
  window_t* window;

  f32 camera_x;
  f32 camera_y;

  char* level;
  i32 level_width;
  i32 level_height;

  i32 visible_tiles_x;
  i32 visible_tiles_y;
  // top-leftmost visible tile
  f32 offset_x;
  f32 offset_y;
} world_t;


char get_tile(world_t* this, i32 x, i32 y) {
  if (x >= 0 && x < this->level_width && y >= 0 && y < this->level_height) {
    return this->level[y * this->level_width + x];
  }
  return ' ';
}

void world_start(world_t* this, window_t* window) {
  this->level = (
    "................................................................"
    ".......................GGGGGGGG................................."
    ".......ooooo...................................................."
    "........ooo....................................................."
    ".......................########................................."
    ".....BB?BBBB?BB.......###..............#.#......................"
    "....................###................#.#......................"
    "...................####........................................."
    "####################################.##############.....########"
    "...................................#.#...............###........"
    "........................############.#............###..........."
    "........................#............#.........###.............."
    "........................#.############......###................."
    "........................#................###...................."
    "........................#################......................."
    "................................................................"
    );
  this->level_width = 64;
  this->level_height = 16;
  this->window = window;
}
void world_on_resize(world_t* this) {
  u16 window_width = window_get_width(this->window);
  u16 window_height = window_get_height(this->window);

  this->visible_tiles_x = ((f32)window_width / world_tile_size) + .5f;
  this->visible_tiles_y = ((f32)window_height / world_tile_size) + .5f;
}
void world_render(world_t* this) {
  // calculate top-leftmost visible tile
  this->offset_x = this->camera_x - (f32)this->visible_tiles_x / 2.f;
  this->offset_y = this->camera_y - (f32)this->visible_tiles_y / 2.f;
  // clamp camera to game boudaries
  this->offset_x = math_clamp_f32(
    this->offset_x,
    0.f, math_max_f32(0.f, this->level_width - this->visible_tiles_x)
  );
  this->offset_y = math_clamp_f32(
    this->offset_y,
    0.f, math_max_f32(0.f, this->level_height - this->visible_tiles_y)
  );
  // Get offsets for smooth movement
  f32 tile_world_offset_x = (this->offset_x - (i32)this->offset_x) * world_tile_size;
  f32 tile_world_offset_y = (this->offset_y - (i32)this->offset_y) * world_tile_size;
  // draw visible tile map
  for (i32 x = -1; x < this->visible_tiles_x + 1; x++) {
    for (i32 y = -1; y < this->visible_tiles_y + 1; y++) {
      char tile_id = get_tile(this, x + this->offset_x, y + this->offset_y);
      gfx_rect_t tile_props = {
        .rect = {
          x * world_tile_size - tile_world_offset_x,
          y * world_tile_size - tile_world_offset_y,
        },
        .color = {.a = 1.f }
      };
      rect_set_size(&tile_props.rect, world_tile_size);
      switch (tile_id) {
        case '.': // Sky
          tile_props.color.r = 0.f;
          tile_props.color.g = 1.f;
          tile_props.color.b = 1.f;
          break;
        case '#': // Solid Block
          tile_props.color.r = 1.f;
          break;
        default:
          break;
      }
      gfx_draw_rect(this->window, &tile_props);
    }
  }
}