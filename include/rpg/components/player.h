#pragma once

#include <sdk/types.h>

typedef struct local_map_t local_map_t;

typedef struct player_t {
  // props
  local_map_t* map;
  // internal::props
  f32 x, y;
  enum {
    PLAYER_UP,
    PLAYER_DOWN,
    PLAYER_RIGHT,
    PLAYER_LEFT
  } direction;
  enum {
    PLAYER_STATE_STANDING_1,
    PLAYER_STATE_WALKING_1,
    PLAYER_STATE_STANDING_2,
    PLAYER_STATE_WALKING_2,
    PLAYER_STATE_MAX
  } state;
  f32 walking_animation;
  // components
} player_t;