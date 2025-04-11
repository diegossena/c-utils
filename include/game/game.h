#ifndef GAME_H
#define GAME_H

#include <sdk/types.h>

const f32 tile_screen_size = 60.f;

void tile_draw(f32 x0, f32 y0, u8 tile_x, u8 tile_y);
void text_draw(const char* text, f32 size, f32 x, f32 y);
void rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y
);

#endif