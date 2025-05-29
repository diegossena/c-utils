#ifndef TILEMAP_H
#define TILEMAP_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

// #define CHUNK_SIZE 256
#define CHUNK_SIZE 16

#define tilemap_index(x, y, layer) layer * global_tilemap->rendered_area + y * global_tilemap->rendered_tiles_x + x
#define tilemap_tile(x, y, layer) global_tilemap->screen_tiles[tilemap_index(x, y, layer)]

typedef enum tile_flag_t {
  TILE_X_FLIP = 1 << 0,
  TILE_Y_FLIP = 1 << 1,
  TILE_SOLID = 1 << 2
} tile_flag_t;

typedef struct tile_t {
  u16 id;
  u8 flags; // tile_flag_t
} tile_t;

typedef enum chunk_position_t {
  // X Y
  CHUNK_TOP_LEFT,
  CHUNK_TOP,
  CHUNK_TOP_RIGHT,
  CHUNK_LEFT,
  CHUNK_CENTER,
  CHUNK_RIGHT,
  CHUNK_BOTTOM_LEFT,
  CHUNK_BOTTOM,
  CHUNK_BOTTOM_RIGHT,
  // Z
  CHUNK_UP,
  CHUNK_DOWN,
  // MAX
  CHUNK_MAX
} chunk_position_t;

typedef enum layer_t {
  LAYER_0,
  LAYER_1,
  LAYER_2,
  LAYER_MAX
} layer_t;

typedef struct chunk_t {
  i8 x, y;
  i8 z;
  tile_t tiles[CHUNK_SIZE][CHUNK_SIZE][LAYER_MAX]; // [x][y][layer]
} chunk_t;

typedef struct tilemap_t {
  // world
  f32 x, y;
  i8 z;
  // camera_movement
  f32 start_x, start_y;
  f32 target_x, target_y;
  f32 move_timer;
  f32 move_duration;
  bool moving;
  // player
  bool player_hidden;
  u8 player_direction; // key_t
  enum {
    PLAYER_STATE_STANDING_1,
    PLAYER_STATE_WALKING_1,
    PLAYER_STATE_STANDING_2,
    PLAYER_STATE_WALKING_2,
    PLAYER_STATE_MAX
  } moving_state;
  // render
  f32 visible_tiles_x;
  f32 visible_tiles_y;
  u8 rendered_tiles_x;
  u8 rendered_tiles_y;
  u16 rendered_area;
  u16 rendered_total;
  u8 tile_size;
  /**
   * [10] Z+1
   * [1][2][3]
   * [4][0][6]
   * [7][8][9]
   * [11] Z-1
   */
  chunk_t chunks[CHUNK_MAX];
  tile_t** screen_tiles; // [z][y][x]
  f32 tile_ndc_pixel_x;
  f32 tile_ndc_pixel_y;
} tilemap_t;

void tilemap_load();
void tilemap_unload();
void tilemap_onkeypress();
void tilemap_onresize();
void tilemap_draw();
void tilemap_set_center(f32 x, f32 y);
void tilemap_chunk_load();

inline void tilemap_moveto(f32 x, f32 y, f32 duration);
void tilemap_move(f32 x, f32 y);

inline tile_t* tile_from_screen();

extern tilemap_t* global_tilemap;

#endif