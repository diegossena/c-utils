#ifndef TILEMAP_H
#define TILEMAP_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

#define TILEMAP_LAYERS 3
#define TILE_SIZE 30

#define tilemap_index(layer, tile_x, tile_y) (layer * tilemap->area + tile_y * tilemap->width + tile_x)
#define tilemap_tile(layer, tile_x, tile_y) tilemap->tiles[tilemap_index(layer, tile_x, tile_y)]

typedef enum tile_flag_t {
  TILE_X_FLIP = 1 << 0,
  TILE_Y_FLIP = 1 << 1
} tile_flag_t;

typedef struct tile_t {
  u8 id;
  u8 flags;
} tile_t;

typedef struct tilemap_t {
  u8 layers;
  u8 width;
  u8 height;
  u16 area;
  tile_t* tiles;
  // camera_movement
  vec2_f32_t start_offset;
  vec2_f32_t target_offset;
  f32 move_timer;
  f32 move_duration;
  bool moving;
  // player
  bool player_hidden;
  key_t player_direction;
  enum {
    PLAYER_STATE_STANDING_1,
    PLAYER_STATE_WALKING_1,
    PLAYER_STATE_STANDING_2,
    PLAYER_STATE_WALKING_2,
    PLAYER_STATE_MAX
  } moving_state;
  // rendering
  f32 tile_ndc_pixel[2];
  // camera
  vec2_f32_t offset;
  f32 visible_tiles[2];
  u8 rendered_blocks[2];
} tilemap_t;

export void tilemap_load();
export void tilemap_unload();
export void tilemap_onkeypress();
export void tilemap_onresize();
export void tilemap_draw();
export void tilemap_set_player(f32 x, f32 y);
export void tilemap_moveto(f32 x, f32 y);
export void tilemap_copy(const tile_t* tiles, u8 width, u8 height);

export vec2_i32_t tilemap_tile_from_screen();

extern tilemap_t* tilemap;

#endif