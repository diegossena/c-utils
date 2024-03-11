#pragma once

#include <sdk/sdk.h>

i32 tile_size = 32;

f32 camera_x;
f32 camera_y;

char* world_level;
i32 world_level_width;
i32 world_level_height;

i32 visible_tiles_x;
i32 visible_tiles_y;
// top-leftmost visible tile
f32 world_offset_x;
f32 world_offset_y;

char get_tile(i32 x, i32 y) {
  if (x >= 0 && x < world_level_width && y >= 0 && y < world_level_height) {
    return world_level[y * world_level_width + x];
  }
  return ' ';
}

void world_start() {
  world_level = (
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
  world_level_width = 64;
  world_level_height = 16;
}
void world_on_resize(window_t* window) {
  u16 window_width = window_get_screen_width(window);
  u16 window_height = window_get_screen_height(window);

  visible_tiles_x = ((f32)window_width / tile_size) + .5f;
  visible_tiles_y = ((f32)window_height / tile_size) + .5f;
}
void world_set_camera(f32 x, f32 y) {
  camera_x = x;
  camera_y = x;
}
void world_render(window_t* window) {
  // calculate top-leftmost visible tile
  world_offset_x = camera_x - (f32)visible_tiles_x / 2.f;
  world_offset_y = camera_y - (f32)visible_tiles_y / 2.f;
  // clamp camera to game boudaries
  world_offset_x = math_clamp_f32(
    world_offset_x,
    0.f, math_max_f32(0.f, world_level_width - visible_tiles_x)
  );
  world_offset_y = math_clamp_f32(
    world_offset_y,
    0.f, math_max_f32(0.f, world_level_height - visible_tiles_y)
  );
  // Get offsets for smooth movement
  f32 tile_world_offset_x = (world_offset_x - (i32)world_offset_x) * tile_size;
  f32 tile_world_offset_y = (world_offset_y - (i32)world_offset_y) * tile_size;
  // draw visible tile map
  for (i32 x = -1; x < visible_tiles_x + 1; x++) {
    rect_props_t rect_props = {
      .rect = { 10.f, 10.f, .size = 100.f},
      .color = { 0.f, 1.f, 0.f, 1.f }
    };
    for (i32 y = -1; y < visible_tiles_y + 1; y++) {
      char tile_id = get_tile(x + world_offset_x, y + world_offset_y);
      rect_props_t tile_props = {
        .rect = {
          .left = x * tile_size - tile_world_offset_x,
          .top = y * tile_size - tile_world_offset_y,
          .size = tile_size
        },
        .color = {.a = 1.f }
      };
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
      gfx_draw_rect(window, &tile_props);
    }
  }
}