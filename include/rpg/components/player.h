#pragma once

#include <sdk/types.h>

typedef struct local_map_t local_map_t;

typedef enum player_state_t {
  PLAYER_STATE_STANDING_1,
  PLAYER_STATE_WALKING_1,
  PLAYER_STATE_STANDING_2,
  PLAYER_STATE_WALKING_2,
  PLAYER_STATE_MAX
} player_state_t;

typedef struct player_t {
  // props
  local_map_t* map;
  // internal::props
  f32 x, y;
  // walking animation
  f32 start_x, start_y;
  f32 distance_x, distance_y;
  enum {
    PLAYER_UP,
    PLAYER_DOWN,
    PLAYER_RIGHT,
    PLAYER_LEFT
  } direction;
  player_state_t state;
  player_state_t start_state;
  f32 walking_timer;
  f32 duration;
  // components
} player_t;