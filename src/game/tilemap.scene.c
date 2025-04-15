#include <game/game.h>
#include <game/tilemap.scene.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#define TILEMAP_VERTICES_USED 4
#define TILEMAP_INDEXES_USED 6

tilemap_t tilemap;

export void tilemap_set_player(f32 x, f32 y) {
  tilemap.offset[0] = x - tilemap.visible_tilesf[0] / 2 + .5f;
  tilemap.offset[1] = y - tilemap.visible_tilesf[1] / 2 + .5f;
}

export void tilemap_load() {
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
  tilemap.rendered_tiles[0] = 0;
  tilemap.rendered_tiles[1] = 0;
  tilemap_onresize();
  // loaded
  tilemap.loaded = true;
  window_updated = true;
}
export void tilemap_unload() {
  tilemap.loaded = false;
}
export void tilemap_onresize() {
  tilemap.visible_tilesf[0] = (f32)window_width / TILE_SIZE;
  tilemap.visible_tilesf[1] = (f32)window_height / TILE_SIZE;
  u64 tilemap_vertices_capacity = vertices_capacity - tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 4 + TILEMAP_VERTICES_USED;
  u64 tilemap_indexes_capacity = indexes_capacity - tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 6 + TILEMAP_INDEXES_USED;
  tilemap.rendered_tiles[0] = math_ceil(tilemap.visible_tilesf[0]) + 1;
  tilemap.rendered_tiles[1] = math_ceil(tilemap.visible_tilesf[1]) + 1;
  tilemap_vertices_capacity += tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 4 + TILEMAP_VERTICES_USED;
  tilemap_indexes_capacity += tilemap.rendered_tiles[1] * tilemap.rendered_tiles[1] * 6 + TILEMAP_INDEXES_USED;
  if (tilemap_vertices_capacity != vertices_capacity) {
    vertices_alloc(tilemap_vertices_capacity);
    indexes_alloc(tilemap_indexes_capacity);
  }
  tilemap.tile_ndc_per_px[0] = TILE_SIZE * ndc_per_px_x;
  tilemap.tile_ndc_per_px[1] = TILE_SIZE * ndc_per_px_y;
}
export void tilemap_onkeypress() {
  if (tilemap.moving == false) {
    // camera_moveto
    if (window_key_pressed(KEY_UP)) {
      tilemap.target_offset[0] = tilemap.offset[0];
      tilemap.target_offset[1] = tilemap.offset[1] - 1.f;
      tilemap.player_direction = KEY_UP;
      tilemap.moving = true;
    } else if (window_key_pressed(KEY_DOWN)) {
      tilemap.target_offset[0] = tilemap.offset[0];
      tilemap.target_offset[1] = tilemap.offset[1] + 1.f;
      tilemap.player_direction = KEY_DOWN;
      tilemap.moving = true;
    } else if (window_key_pressed(KEY_LEFT)) {
      tilemap.target_offset[0] = tilemap.offset[0] - 1.f;
      tilemap.target_offset[1] = tilemap.offset[1];
      tilemap.player_direction = KEY_LEFT;
      tilemap.moving = true;
    } if (window_key_pressed(KEY_RIGHT)) {
      tilemap.target_offset[0] = tilemap.offset[0] + 1.f;
      tilemap.target_offset[1] = tilemap.offset[1];
      tilemap.player_direction = KEY_RIGHT;
      tilemap.moving = true;
    }
    if (tilemap.moving) {
      tilemap.move_timer = 0;
      tilemap.move_duration = .5f;
      tilemap.start_offset[0] = tilemap.offset[0];
      tilemap.start_offset[1] = tilemap.offset[1];
      window_updated = true;
    }
  }
}
export void tilemap_render() {
  // camera_moveto_render
  if (tilemap.moving) {
    tilemap.move_timer += window_deltatime;
    f32 progress = tilemap.move_timer / tilemap.move_duration;
    if (progress > 1.f) {
      progress = 1.f;
      tilemap.moving = false;
    }
    tilemap.offset[0] = (tilemap.target_offset[0] - tilemap.start_offset[0]) * progress + tilemap.start_offset[0];
    tilemap.offset[1] = (tilemap.target_offset[1] - tilemap.start_offset[1]) * progress + tilemap.start_offset[1];
    window_updated = true;
  }
  // Get offsets for smooth movement
  const i8 start_x = (i8)math_floor(tilemap.offset[0]);
  const i8 start_y = (i8)math_floor(tilemap.offset[1]);
  f32 tile_offset[2] = {
    tilemap.offset[0] - start_x,
    tilemap.offset[1] - start_y
  };
  f32 x0, y0, x1, y1;
  const f32 start_x0 = -1.f - (tile_offset[0] * tilemap.tile_ndc_per_px[0]);
  const f32 start_y0 = 1.f + (tile_offset[1] * tilemap.tile_ndc_per_px[1]);
  const i8 end_x = start_x + tilemap.rendered_tiles[0];
  const i8 end_y = start_y + tilemap.rendered_tiles[1];
  x0 = start_x0;
  for (i8 x = start_x, i = 0; x < end_x; x++, i++) {
    x1 = x0 + tilemap.tile_ndc_per_px[0];
    y0 = start_y0;
    for (i8 y = start_y; y < end_y; y++) {
      y1 = y0 - tilemap.tile_ndc_per_px[1];
      for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
        u8 tile_id = (x >= 0 && x < TILEMAP_WIDTH) && (y >= 0 && y < TILEMAP_WIDTH)
          ? tilemap.tiles[layer][y * TILEMAP_WIDTH + x]
          : 0;
        if (tile_id != 0) {
          u8 tile_x = (tile_id - 1) % 10;
          u8 tile_y = math_ceil((f32)tile_id / 10.f) - 1.f;
          tile_draw(x0, y0, x1, y1, tile_x, tile_y, false, false);
        }
      }
      y0 = y1;
    }
    x0 = x1;
  }
  // player render
  x0 = -tilemap.tile_ndc_per_px[0] / 2;
  y0 = tilemap.tile_ndc_per_px[1] / 2;
  x1 = -x0;
  y1 = -y0;
  window_rect_fill(
    x0, y0, x1, y1,
    1, 0, 0, 1
  );
}