#ifndef TILEMAP_H
#define TILEMAP_H

#include <sdk/types.h>
#include <sdk/keyboard.h>
#include <sdk/gfx.image.h>

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILE_SIZE 76
#define TILEMAP_LAYERS 3

typedef struct tilemap_t {
  bool loaded;
  gfx_image_t pallet_town_interiors;
  u8 tiles[3][TILEMAP_SIZE];
  // player
  gfx_image_t character_img;
  f32 player[2];
  enum {
    PLAYER_UP,
    PLAYER_DOWN,
    PLAYER_RIGHT,
    PLAYER_LEFT
  } player_direction;
  enum {
    PLAYER_STATE_STANDING_1,
    PLAYER_STATE_WALKING_1,
    PLAYER_STATE_STANDING_2,
    PLAYER_STATE_WALKING_2,
    PLAYER_STATE_MAX
  } player_walking_state;
  f32 player_walking_timer;
  f32 player_walking_duration;
  // camera
  u8 visible_tiles_x, visible_tiles_y;
  f64 offset_limit[2];
} tilemap_t;

export void tilemap_load();
export void tilemap_unload();

export void tilemap_render();

export void tilemap_onkeypress();
export void tilemap_render();

extern tilemap_t tilemap;

#endif