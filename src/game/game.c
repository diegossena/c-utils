#include <game/game.h>
#include <game/tilemap.scene.h>
#include <sdk/window.h>

#define ATLAS_TILE_SIZE 8

const u16 atlas_width = 256;
const u16 atlas_height = 256;
const u16 atlas_tiles_in_x = atlas_width / ATLAS_TILE_SIZE;
const u16 atlas_tiles_in_y = atlas_height / ATLAS_TILE_SIZE;

export void tile_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y,
  u8 flags
) {
  const f32 atlas_ngc_per_px = (1.f / atlas_width) * ATLAS_TILE_SIZE;
  const f32 u0 = atlas_ngc_per_px * tile_x;
  const f32 v0 = atlas_ngc_per_px * tile_y;
  const f32 u1 = u0 + atlas_ngc_per_px;
  const f32 v1 = v0 + atlas_ngc_per_px;
  window_rect_draw(
    x0, y0, x1, y1,
    flags & TILE_X_FLIP ? u1 : u0,
    flags & TILE_Y_FLIP ? v1 : v0,
    flags & TILE_X_FLIP ? u0 : u1,
    flags & TILE_Y_FLIP ? v0 : v1
  );
}
export void text_draw(const char* text, f32 size, f32 x0, f32 y0) {
  const f32 spacing = 8 * window_pixel_ndc[0];
  const f32 y1 = y0 - window_pixel_ndc[1] * size;
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
      const f32 x1 = x0 + window_pixel_ndc[0] * size;
      // draw
      tile_draw(x0, y0, x1, y1, tile_x, tile_y, 0);
      // spacing
      x0 += spacing;
    } else {
      x0 += spacing;
    }
    ++text;
  }
}