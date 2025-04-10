#ifndef GAME_H
#define GAME_H

#include <sdk/types.h>

void rect_fill_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 r, f32 g, f32 b, f32 a
);
void text_draw(const char* text, f32 size, f32 x, f32 y);
void tile_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y
);

#endif