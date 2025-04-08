#ifndef TILEMAP_H
#define TILEMAP_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 76

typedef struct tilemap_t {
  bool loaded;
  u8 tiles[3][TILEMAP_SIZE];
  // camera
  u8 visible_tiles_x, visible_tiles_y;
  f32 offset[2];
  u8 tile_offset_x, tile_offset_y;
  f32 offset_limit[2];
} tilemap_t;

export void tilemap_load();
export void tilemap_unload();

export void tilemap_draw();
export u8 tilemap_tiles_get(const u8* tiles, i32 x, i32 y);

export void tilemap_onkeydown(key_code_t key);
export void tilemap_render();

extern tilemap_t tilemap;

#endif