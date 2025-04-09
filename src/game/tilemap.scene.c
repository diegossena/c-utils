#include <game/tilemap.scene.h>
#include <sdk/window.h>

tilemap_t tilemap;

export void tilemap_load() {
  f32 visible_tiles_x = (f32)global_window_width / TILE_SIZE;
  f32 visible_tiles_y = (f32)global_window_height / TILE_SIZE;
  tilemap.visible_tiles_x = math_ceil(visible_tiles_x);
  tilemap.visible_tiles_y = math_ceil(visible_tiles_y);
  tilemap.offset_limit[0] = (f32)TILEMAP_WIDTH - visible_tiles_x;
  tilemap.offset_limit[1] = (f32)TILEMAP_WIDTH - visible_tiles_y;
  // pallet_town_interiors
  gfx_image_src(&tilemap.pallet_town_interiors, L"assets/pallet_town_interiors.png");
  // loaded
  tilemap.loaded = true;
}
export void tilemap_unload() {
  gfx_image_free(&tilemap.pallet_town_interiors);
}
export void tilemap_onkeydown(key_code_t key) {}
export u8 tilemap_tiles_get(const u8* tiles, i32 x, i32 y) {
  if (x >= 0 && x < TILEMAP_WIDTH && y >= 0 && y < TILEMAP_WIDTH) {
    return tiles[y * TILEMAP_WIDTH + x];
  }
  return -1;
}
export void tilemap_render() {
  // pallet_town_interiors
  const f32 pallet_town_interiors_rect[4] = { 0, 0, 76, 76 };
  const f32 pallet_town_interiors_src_rect[4] = { 5, 5, 76, 76 };
  gfx_image_draw(
    &tilemap.pallet_town_interiors, pallet_town_interiors_rect,
    pallet_town_interiors_src_rect, 1.f
  );
}