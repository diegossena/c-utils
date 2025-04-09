#include <game/tilemap.scene.h>
#include <sdk/window.h>

tilemap_t tilemap;

export void tilemap_load() {
  // f32 visible_tiles_x = (f32)global_window_width / TILE_SIZE;
  // f32 visible_tiles_y = (f32)global_window_height / TILE_SIZE;
  // tilemap.visible_tiles_x = math_ceil(visible_tiles_x);
  // tilemap.visible_tiles_y = math_ceil(visible_tiles_y);
  // tilemap.offset_limit[0] = (f32)TILEMAP_WIDTH - visible_tiles_x;
  // tilemap.offset_limit[1] = (f32)TILEMAP_WIDTH - visible_tiles_y;
  // tilemap.offset_limit[0] = math_floor(tilemap.offset_limit[0] * 1000.f) / 1000.f;
  // tilemap.offset_limit[1] = math_floor(tilemap.offset_limit[1] * 1000.f) / 1000.f;
  // // tilemap
  // gfx_image_src(&tilemap.pallet_town_interiors, L"assets/pallet_town_interiors.png");
  // // player
  // gfx_image_src(&tilemap.character_img, L"assets/character.png");
  // // walking animate
  // // loaded
  // tilemap.loaded = true;
  // global_repaint = true;
}
export void tilemap_unload() {
}
export void tilemap_onkeydown(key_code_t key) {
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
          global_window_repaint = true;
        break;
      default:
    }
  }
}
export void tilemap_render() {
  // // calculate top-leftmost visible tile
  // f32 offset[2] = {
  //   tilemap.player[0] - (f32)tilemap.visible_tiles_x / 2.f,
  //   tilemap.player[1] - (f32)tilemap.visible_tiles_y / 2.f
  // };
  // offset[0] = math_floor(offset[0] * 1000.f) / 1000.f;
  // offset[1] = math_floor(offset[1] * 1000.f) / 1000.f;
  // // clamp camera to game boudaries
  // offset[0] = math_clamp(offset[0], 0.f, tilemap.offset_limit[0]);
  // offset[1] = math_clamp(offset[1], 0.f, tilemap.offset_limit[1]);
  // // Get offsets for smooth movement
  // i8 tile_offset_x = (offset[0] - (i8)offset[0]) * TILE_SIZE;
  // i8 tile_offset_y = (offset[1] - (i8)offset[1]) * TILE_SIZE;
  // // layers render
  // for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
  //   for (i8 x = -1; x < tilemap.visible_tiles_x + 1; x++) {
  //     for (i8 y = -1; y < tilemap.visible_tiles_y + 1; y++) {
  //       i8 tile_x = x + offset[0];
  //       i8 tile_y = y + offset[1];
  //       u8 tile_id = (tile_x >= 0 && tile_x < TILEMAP_WIDTH) && (tile_y >= 0 && tile_y < TILEMAP_WIDTH)
  //         ? tilemap.tiles[layer][tile_y * TILEMAP_WIDTH + tile_x]
  //         : 0;
  //       if (tile_id == 0)
  //         continue;
  //       f32 rect[4] = {
  //         x * TILE_SIZE - tile_offset_x,
  //         y * TILE_SIZE - tile_offset_y
  //       };
  //       rect[2] = rect[0] + TILE_SIZE;
  //       rect[3] = rect[1] + TILE_SIZE;
  //       f32 src_rect[4];
  //       if (tile_id <= 49) {
  //         // HERO HOME 2F
  //         switch (tile_id) {
  //           case 1:
  //           case 6:
  //           case 14:
  //           case 28:
  //           case 36:
  //           case 43:
  //             src_rect[0] = 5;
  //             break;
  //           case 2:
  //           case 7:
  //           case 15:
  //           case 23:
  //           case 29:
  //           case 37:
  //           case 44:
  //             src_rect[0] = 21;
  //             break;
  //           case 8:
  //           case 16:
  //           case 30:
  //           case 38:
  //           case 45:
  //             src_rect[0] = 37;
  //             break;
  //           case 3:
  //           case 9:
  //           case 17:
  //           case 31:
  //           case 39:
  //           case 47:
  //             src_rect[0] = 53;
  //             break;
  //           case 4:
  //           case 10:
  //           case 18:
  //           case 32:
  //           case 40:
  //           case 48:
  //             src_rect[0] = 69;
  //             break;
  //           case 11:
  //           case 19:
  //           case 24:
  //           case 33:
  //           case 41:
  //           case 46:
  //             src_rect[0] = 85;
  //             break;
  //           case 34:
  //             src_rect[0] = 101;
  //             break;
  //           case 12:
  //           case 20:
  //           case 25:
  //           case 35:
  //           case 42:
  //           case 49:
  //             src_rect[0] = 117;
  //             break;
  //           case 21:
  //           case 26:
  //             src_rect[0] = 133;
  //             break;
  //           case 22:
  //           case 27:
  //             src_rect[0] = 149;
  //             break;
  //           case 5:
  //           case 13:
  //             src_rect[0] = 165;
  //             break;
  //         }
  //         if (tile_id <= 5) {
  //           src_rect[1] = 5;
  //         } else if (tile_id <= 13) {
  //           src_rect[1] = 21;
  //         } else if (tile_id <= 22) {
  //           src_rect[1] = 37;
  //         } else if (tile_id <= 27) {
  //           src_rect[1] = 53;
  //         } else if (tile_id <= 35) {
  //           src_rect[1] = 69;
  //         } else if (tile_id <= 42) {
  //           src_rect[1] = 85;
  //         } else if (tile_id <= 46) {
  //           src_rect[1] = 101;
  //         } else if (tile_id <= 49) {
  //           src_rect[1] = 117;
  //         }
  //       }
  //       const f32 sprite_size = 16.f;
  //       src_rect[2] = src_rect[0] + sprite_size;
  //       src_rect[3] = src_rect[1] + sprite_size;
  //       gfx_image_draw(&tilemap.pallet_town_interiors, rect, src_rect, 1.f);
  //     }
  //   }
  // }
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