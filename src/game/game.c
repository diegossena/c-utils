#include <game/game.h>

#include <game/game.h>
#include <game/tilemap.h>
#include <sdk/window.h>

const u16 atlas_width = 256;
const u16 atlas_height = 256;
const u16 atlas_tile_size = 8;

const u16 atlas_tiles_width = atlas_width / atlas_tile_size;
const u16 atlas_tiles_height = atlas_height / atlas_tile_size;

void tile_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y,
  u8 flags
) {
  const f32 atlas_ngc_per_px = (1.f / atlas_width) * atlas_tile_size;
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
void text_draw(const char* text, f32 x0, f32 y0, f32 size) {
  const f32 tile_pixel = size * 1 / atlas_tile_size;
  const f32 spacing = (size - tile_pixel * 3) * window_pixel_ndc_x;
  const f32 y1 = y0 - window_pixel_ndc_y * size;
  while (*text != '\0') {
    if (*text >= 'A' && *text <= 'Z') {
      u8 tile_x = *text - 'A';
      const f32 x1 = x0 + window_pixel_ndc_x * size;
      // draw
      tile_draw(x0, y0, x1, y1, tile_x, 0, 0);
    }
    x0 += spacing;
    ++text;
  }
}