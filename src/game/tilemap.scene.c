#include <game/game.h>
#include <game/tilemap.scene.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#define TILEMAP_VERTICES_USED 4
#define TILEMAP_INDEXES_USED 6

tilemap_t tilemap;

export void tilemap_set_player(f32 x, f32 y) {
  tilemap.offset.x = x - tilemap.visible_tiles[0] / 2 + .5f;
  tilemap.offset.y = y - tilemap.visible_tiles[1] / 2 + .5f;
}

export void tilemap_load() {
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
  tilemap.rendered_tiles[0] = 0;
  tilemap.rendered_tiles[1] = 0;
  tilemap.player_direction = KEY_DOWN;
  tilemap_onresize();
  // loaded
  tilemap.loaded = true;
  window_updated = true;
}
export void tilemap_unload() {
  tilemap.loaded = false;
  const u8 rendered_area = tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1];
  vertices_alloc(vertices_capacity - rendered_area * 4 + TILEMAP_VERTICES_USED);
  indexes_alloc(indexes_capacity - rendered_area * 6 + TILEMAP_INDEXES_USED);
}
export void tilemap_onresize() {
  tilemap.visible_tiles[0] = (f32)window_width / TILE_SIZE;
  tilemap.visible_tiles[1] = (f32)window_height / TILE_SIZE;
  u8 rendered_area = tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1];
  u64 tilemap_vertices_capacity = vertices_capacity - rendered_area * 4 + TILEMAP_VERTICES_USED;
  u64 tilemap_indexes_capacity = indexes_capacity - rendered_area * 6 + TILEMAP_INDEXES_USED;
  tilemap.rendered_tiles[0] = math_ceil(tilemap.visible_tiles[0]) + 1;
  tilemap.rendered_tiles[1] = math_ceil(tilemap.visible_tiles[1]) + 1;
  rendered_area = tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1];
  tilemap_vertices_capacity += rendered_area * 4 + TILEMAP_VERTICES_USED;
  tilemap_indexes_capacity += rendered_area * 6 + TILEMAP_INDEXES_USED;
  if (tilemap_vertices_capacity != vertices_capacity) {
    vertices_alloc(tilemap_vertices_capacity);
    indexes_alloc(tilemap_indexes_capacity);
  }
  tilemap.tile_ndc_pixel[0] = TILE_SIZE * window_pixel_ndc[0];
  tilemap.tile_ndc_pixel[1] = TILE_SIZE * window_pixel_ndc[1];
}
export void tilemap_moveto(f32 x, f32 y) {
  tilemap.move_timer = 0;
  tilemap.move_duration = .5f;
  tilemap.start_offset = tilemap.offset;
  tilemap.target_offset.x = x;
  tilemap.target_offset.y = y;
  tilemap.moving = true;
  window_updated = true;
}
export void tilemap_onkeypress() {
  if (tilemap.moving == false) {
    if (window_key_pressed(KEY_UP)) {
      tilemap_moveto(tilemap.offset.x, tilemap.offset.y - 1.f);
      tilemap.player_direction = KEY_UP;
    } else if (window_key_pressed(KEY_DOWN)) {
      tilemap_moveto(tilemap.offset.x, tilemap.offset.y + 1.f);
      tilemap.player_direction = KEY_DOWN;
    } else if (window_key_pressed(KEY_LEFT)) {
      tilemap_moveto(tilemap.offset.x - 1.f, tilemap.offset.y);
      tilemap.player_direction = KEY_LEFT;
    } if (window_key_pressed(KEY_RIGHT)) {
      tilemap_moveto(tilemap.offset.x + 1.f, tilemap.offset.y);
      tilemap.player_direction = KEY_RIGHT;
    }
  }
}
export void tilemap_draw() {
  // moveto_update
  if (tilemap.moving) {
    tilemap.move_timer += window_deltatime;
    f32 progress = tilemap.move_timer / tilemap.move_duration;
    if (progress > 1.f) {
      progress = 1.f;
      tilemap.moving = false;
    }
    tilemap.offset.x = (tilemap.target_offset.x - tilemap.start_offset.x) * progress + tilemap.start_offset.x;
    tilemap.offset.y = (tilemap.target_offset.y - tilemap.start_offset.y) * progress + tilemap.start_offset.y;
    window_updated = true;
  }
  // tilemap_draw
  const i8 start_x = (i8)math_floor(tilemap.offset.x);
  const i8 start_y = (i8)math_floor(tilemap.offset.y);
  const i8 end_x = start_x + tilemap.rendered_tiles[0];
  const i8 end_y = start_y + tilemap.rendered_tiles[1];
  const f32 start_x0 = -1.f - (tilemap.offset.x - start_x) * tilemap.tile_ndc_pixel[0];
  const f32 start_y0 = 1.f + (tilemap.offset.y - start_y) * tilemap.tile_ndc_pixel[1];
  for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
    f32 x0, y0, x1, y1;
    if (layer == 2) {
      // player_draw
      x0 = -tilemap.tile_ndc_pixel[0] / 2;
      y0 = tilemap.tile_ndc_pixel[1] / 2;
      x1 = -x0;
      y1 = -y0;
      window_rect_fill(
        x0, y0, x1, y1,
        1, 0, 0, 1
      );
    }
    x0 = start_x0;
    for (i8 x = start_x; x < end_x; x++) {
      x1 = x0 + tilemap.tile_ndc_pixel[0];
      y0 = start_y0;
      for (i8 y = start_y; y < end_y; y++) {
        y1 = y0 - tilemap.tile_ndc_pixel[1];
        u8 tile_id = (x >= 0 && x < TILEMAP_WIDTH) && (y >= 0 && y < TILEMAP_WIDTH)
          ? tilemap.tiles[layer][y * TILEMAP_WIDTH + x]
          : 0;
        if (tile_id != 0) {
          u8 tile_x = (tile_id - 1) % 10;
          u8 tile_y = math_ceil((f32)tile_id / 10.f) - 1.f;
          tile_draw(x0, y0, x1, y1, tile_x, tile_y, false, false);
        }
        y0 = y1;
      }
      x0 = x1;
    }
  }
}