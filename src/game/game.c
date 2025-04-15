#include <game/game.h>
#include <sdk/window.h>

const u16 atlas_width = 160;
const u16 atlas_height = 160;
#define ATLAS_TILE_SIZE 16

export void tile_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y,
  bool x_flip, bool y_flip
) {
  const f32 atlas_ngc_per_px = (1.f / atlas_width) * ATLAS_TILE_SIZE;
  const f32 u0 = atlas_ngc_per_px * tile_x;
  const f32 v0 = atlas_ngc_per_px * tile_y;
  const f32 u1 = u0 + atlas_ngc_per_px;
  const f32 v1 = v0 + atlas_ngc_per_px;
  window_rect_draw(
    x0, y0, x1, y1,
    x_flip ? u1 : u0,
    y_flip ? v1 : v0,
    x_flip ? u0 : u1,
    y_flip ? v0 : v1
  );
}
export void text_draw(const char* text, f32 size, f32 x0, f32 y0) {
  const f32 spacing = size * ndc_per_px_x * .31f;
  const f32 y1 = y0 - ndc_per_px_y * size;
  while (*text != '\0') {
    u8 tile_x = 0, tile_y = 0;
    if (*text >= 'A' && *text <= 'B') {
      tile_x = *text - 57;
      tile_y = 4;
    } else if (*text >= 'C' && *text <= 'L') {
      tile_x = *text - 'C';
      tile_y = 5;
    } else if (*text >= 'M' && *text <= 'V') {
      tile_x = *text - 'M';
      tile_y = 6;
    } else if (*text >= 'W' && *text <= 'Z') {
      tile_x = *text - 'W';
      tile_y = 7;
    } else {
      switch (*text) {
        case '.':
          tile_x = 4;
          tile_y = 7;
          break;
        case ',':
          tile_x = 5;
          tile_y = 7;
          break;
      }
    }
    if (*text != ' ') {
      const f32 x1 = x0 + ndc_per_px_x * size;
      // draw
      tile_draw(x0, y0, x1, y1, tile_x, tile_y, false, false);
      // spacing
      x0 += spacing;
    } else {
      x0 += spacing * .5f;
    }
    ++text;
  }
}