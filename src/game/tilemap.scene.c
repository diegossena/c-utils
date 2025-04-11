#include <game/game.h>
#include <game/tilemap.scene.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

tilemap_t tilemap;

export void tilemap_load() {
  // camera
  f32 visible_tiles_x = (f32)window_width / TILE_SIZE;
  f32 visible_tiles_y = (f32)window_height / TILE_SIZE;
  tilemap.visible_tiles_x = math_ceil(visible_tiles_x) + 1;
  tilemap.visible_tiles_y = math_ceil(visible_tiles_y) + 1;
  // loaded
  tilemap.loaded = true;
  window_has_update = true;
}
export void tilemap_unload() {
  tilemap.loaded = false;
}
export void tilemap_onkeydown(key_code_t key) {
  if (key == KEY_UP) {
    tilemap.player[1] -= .1f;
    window_has_update = true;
  } else if (key == KEY_DOWN) {
    tilemap.player[1] += .1f;
    window_has_update = true;
  }
  if (key == KEY_LEFT) {
    tilemap.player[0] -= .1f;
    window_has_update = true;
  } else if (key == KEY_RIGHT) {
    tilemap.player[0] += .1f;
    window_has_update = true;
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
        tilemap.player_walking_duration =
          window_has_update = true;
        break;
      default:
    }
  }
}
export void tilemap_render() {
  // calculate top-leftmost visible tile
  f32 offset[2] = {
    tilemap.player[0] - (f32)tilemap.visible_tiles_x / 2.f,
    tilemap.player[1] - (f32)tilemap.visible_tiles_y / 2.f
  };
  // offset[0] = math_floor(offset[0] * 100.f) / 100.f;
  // offset[1] = math_floor(offset[1] * 100.f) / 100.f;

  // Get offsets for smooth movement
  f32 tile_offset[2] = {
    (offset[0] - math_floor(offset[0])),
    (offset[1] - math_floor(offset[1]))
  };
  const f32 tile_ndc_per_px[2] = {
    TILE_SIZE * ndc_per_px_x,
    TILE_SIZE * ndc_per_px_y
  };

  const f32 x0_start = -1.f - (tile_offset[0] * tile_ndc_per_px[0]);
  const f32 y0_start = 1.f + (tile_offset[1] * tile_ndc_per_px[1]);

  if (!transition.loading) {
    console_log("offset %f %f", offset[0], offset[1]);
    console_log("(i32)offset %d %d", (i32)math_floor(offset[0]), (i32)math_floor(offset[1]));
    console_log("tile_offset %f %f", tile_offset[0], tile_offset[1]);
    console_log("ndc_per_px %f %f", ndc_per_px_x, ndc_per_px_y);
    console_log("x0_start %f", x0_start);
    console_log("y0_start %f", y0_start);
    console_log("visible_tiles %d %d", tilemap.visible_tiles_x, tilemap.visible_tiles_y);
  }
  // layers render
  for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
    f32 x0 = x0_start;
    for (u8 x = 0; x < tilemap.visible_tiles_x; x++) {
      const f32 x1 = x0 + tile_ndc_per_px[0];
      f32 y0 = y0_start;
      for (u8 y = 0; y < tilemap.visible_tiles_y; y++) {
        const f32 y1 = y0 - tile_ndc_per_px[1];
        // tile_id
        i8 map_tile_x = math_floor(x + offset[0]);
        i8 map_tile_y = math_floor(y + offset[1]);
        u8 tile_id = (map_tile_x >= 0 && map_tile_x < TILEMAP_WIDTH) && (map_tile_y >= 0 && map_tile_y < TILEMAP_WIDTH)
          ? tilemap.tiles[layer][map_tile_y * TILEMAP_WIDTH + map_tile_x]
          : 0;
        if (tile_id != 0) {
          // tile_draw
          u8 tile_x = (tile_id - 1) % 10;
          u8 tile_y = math_ceil((f32)tile_id / 10.f) - 1.f;
          if (!transition.loading && layer == 0 && y == 1) {
            console_log("xy(%d %d) tile_id=%d tile_draw(%f %f %f %f) sprite(%d %d)",
              x, y,
              tile_id,
              x0, x1, y0, y1,
              tile_x, tile_y
            );
          }
          tile_rect_draw(x0, y0, x1, y1, tile_x, tile_y);
        }
        y0 = y1;
      }
      x0 = x1;
    }
  }
  if (!transition.loading) {
    console_log();
  }
  // // player render
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