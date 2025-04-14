#include <game/game.h>
#include <game/tilemap.scene.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

tilemap_t tilemap;

export void tilemap_load() {
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
  tilemap.visible_tilesf[0] = (f32)global_window_width / TILE_SIZE;
  tilemap.visible_tilesf[1] = (f32)global_window_height / TILE_SIZE;
  u64 tilemap_vertices_capacity = vertices_capacity - tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 4;
  u64 tilemap_indexes_capacity = indexes_capacity - tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 6;
  tilemap.rendered_tiles[0] = math_ceil(tilemap.visible_tilesf[0]) + 1;
  tilemap.rendered_tiles[1] = math_ceil(tilemap.visible_tilesf[1]) + 1;
  tilemap_vertices_capacity += tilemap.rendered_tiles[0] * tilemap.rendered_tiles[1] * 4;
  tilemap_indexes_capacity += tilemap.rendered_tiles[1] * tilemap.rendered_tiles[1] * 6;
  if (tilemap_vertices_capacity != vertices_capacity) {
    vertices_alloc(tilemap_vertices_capacity);
    indexes_alloc(tilemap_indexes_capacity);
  }
  tilemap.tile_ndc_per_px[0] = TILE_SIZE * ndc_per_px_x;
  tilemap.tile_ndc_per_px[1] = TILE_SIZE * ndc_per_px_y;
}
export void tilemap_onkeydown(key_code_t key) {
  const f32 speed = .1f;
  if (key == KEY_UP) {
    tilemap.player[1] -= speed;
    window_updated = true;
  } else if (key == KEY_DOWN) {
    tilemap.player[1] += speed;
    window_updated = true;
  }
  if (key == KEY_LEFT) {
    tilemap.player[0] -= speed;
    window_updated = true;
  } else if (key == KEY_RIGHT) {
    tilemap.player[0] += speed;
    window_updated = true;
  }
  return;
  if (tilemap.player_walking == false) {
    switch (key) {
      case KEY_UP:
      case KEY_DOWN:
      case KEY_LEFT:
      case KEY_RIGHT:
        tilemap.player_walking = true;
        tilemap.player_direction = key;
        tilemap.player_walking_timer = 0;
        tilemap.player_walking_duration = 1.f;
        window_updated = true;
        break;
      default:
    }
  }
}
export void tilemap_render() {
  const f32 offset[2] = {
    tilemap.player[0],
    tilemap.player[1]
  };
  // Get offsets for smooth movement
  const f32 tile_offset[2] = {
    (offset[0] - math_floor(offset[0])),
    (offset[1] - math_floor(offset[1]))
  };
  // layers_draw
  const f32 start_x0 = -1.f - (tile_offset[0] * tilemap.tile_ndc_per_px[0]);
  const f32 start_y0 = 1.f + (tile_offset[1] * tilemap.tile_ndc_per_px[1]);
  const i8 start_x = (i8)math_floor(offset[0]);
  const i8 start_y = (i8)math_floor(offset[1]);
  const i8 end_x = start_x + tilemap.rendered_tiles[0];
  const i8 end_y = start_y + tilemap.rendered_tiles[1];
  for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
    f32 x0 = start_x0;
    for (i8 x = start_x; x < end_x; x++) {
      const f32 x1 = x0 + tilemap.tile_ndc_per_px[0];
      f32 y0 = start_y0;
      for (i8 y = start_y; y < end_y; y++) {
        const f32 y1 = y0 - tilemap.tile_ndc_per_px[1];
        // tile_id
        u8 tile_id = (x >= 0 && x < TILEMAP_WIDTH) && (y >= 0 && y < TILEMAP_WIDTH)
          ? tilemap.tiles[layer][y * TILEMAP_WIDTH + x]
          : 0;
        if (!transition.loading && layer == 0 && x == 0) {
          console_log(
            "[%d %d] "
            "rect %f %f %f %f "
            "tile_id %d ",
            x, y,
            x0, x1, y0, y1,
            tile_id
          );
        }
        if (tile_id != 0) {
          // tile
          u8 tile_x = (tile_id - 1) % 10;
          u8 tile_y = math_ceil((f32)tile_id / 10.f) - 1.f;
          tile_draw(x0, y0, x1, y1, tile_x, tile_y, false, false);
        }
        y0 = y1;
      }
      x0 = x1;
    }
  }
  if (!transition.loading) {
    console_log();
  }
  // player render
  // const f32 scale = 4.f;
  // f32 rect[4] = {
  //   (tilemap.player[0] - offset[0]) * TILE_SIZE + 6.f,
  //   (tilemap.player[1] - offset[1]) * TILE_SIZE + -58.f,
  // };
  // rect[2] = rect[0] + 15.f * scale;
  // rect[3] = rect[1] + 31.f * scale;
  // f32 src_rect[4];
  // switch (tilemap.player_walking_state) {
  //   case PLAYER_STATE_STANDING_1:
  //   case PLAYER_STATE_STANDING_2:
  //     src_rect[0] = 25;
  //     break;
  //   case PLAYER_STATE_WALKING_1:
  //     src_rect[0] = 8;
  //     break;
  //   case PLAYER_STATE_WALKING_2:
  //     src_rect[0] = 42;
  //     break;
  // }
  // switch (tilemap.player_direction) {
  //   case PLAYER_UP:
  //     src_rect[1] = 75;
  //     break;
  //   case PLAYER_DOWN:
  //     src_rect[1] = 42;
  //     break;
  //   case PLAYER_RIGHT:
  //     src_rect[1] = 141;
  //     break;
  //   case PLAYER_LEFT:
  //     src_rect[1] = 108;
  //     break;
  // }
  // src_rect[2] = src_rect[0] + 15.f;
  // src_rect[3] = src_rect[1] + 31.f;
  // gfx_image_draw(&tilemap.character_img, rect, src_rect, 1.f);
}