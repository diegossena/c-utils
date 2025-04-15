#ifndef TILEMAP_H
#define TILEMAP_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

#define TILEMAP_WIDTH 60
#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_WIDTH)
#define TILEMAP_LAYERS 3

typedef struct tilemap_t {
  bool loaded;
  u8 tiles[3][TILEMAP_SIZE];
  // camera_movement
  f32 start_offset[2];
  f32 target_offset[2];
  f32 move_timer;
  f32 move_duration;
  bool moving;
  // player
  key_code_t player_direction;
  enum {
    PLAYER_STATE_STANDING_1,
    PLAYER_STATE_WALKING_1,
    PLAYER_STATE_STANDING_2,
    PLAYER_STATE_WALKING_2,
    PLAYER_STATE_MAX
  } moving_state;
  // rendering
  f32 tile_ndc_per_px[2];
  // camera
  f32 offset[2];
  f32 visible_tilesf[2];
  u8 rendered_tiles[2];
} tilemap_t;

export void tilemap_load();
export void tilemap_unload();
export void tilemap_onkeypress();
export void tilemap_onresize();
export void tilemap_render();
export void tilemap_set_player(f32 x, f32 y);

extern tilemap_t tilemap;

#endif