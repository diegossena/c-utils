#include <game/tilemap.scene.h>
#include <sdk/window.h>

tilemap_t tilemap;

export void tilemap_load() {
  f32 visible_tiles_x = (f32)global_window_width / TILE_SIZE;
  f32 visible_tiles_y = (f32)global_window_height / TILE_SIZE;
  tilemap.visible_tiles_x = math_ceil(visible_tiles_x);
  tilemap.visible_tiles_y = math_ceil(visible_tiles_y);
  tilemap.offset_limit.x = (f32)TILEMAP_WIDTH - visible_tiles_x;
  tilemap.offset_limit.y = (f32)TILEMAP_WIDTH - visible_tiles_y;

  tilemap.loaded = true;
}
export u8 tilemap_tiles_get(const u8* tiles, i32 x, i32 y) {
  if (x >= 0 && x < TILEMAP_WIDTH && y >= 0 && y < TILEMAP_WIDTH) {
    return tiles[y * TILEMAP_WIDTH + x];
  }
  return -1;
}
export void tilemap_draw() {

}