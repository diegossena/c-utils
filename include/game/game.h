#ifndef GAME_H
#define GAME_H

#include <sdk/types.h>

#define TILE_SIZE 60

typedef enum transition_state_t {
  TRANSITION_ENTERING,
  TRANSITION_ENTERED,
  TRANSITION_EXITING,
  TRANSITION_EXITED
} transition_state_t;

export void text_draw(const char* text, f32 size, f32 x, f32 y);
export void tile_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y,
  bool x_flip, bool y_flip
);

#endif