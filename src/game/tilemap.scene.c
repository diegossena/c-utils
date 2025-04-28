#include <game/game.h>
#include <game/tilemap.scene.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#define TILEMAP_VERTICES_USED 4
#define TILEMAP_INDEXES_USED 6

tilemap_t tilemap = { .player_direction = KEY_DOWN };

export void tilemap_copy(const tileblock_t* tiles, u8 width, u8 height) {
  const u16 area = width * height;
  const u64 capacity = area * sizeof(tileblock_t) * TILEMAP_LAYERS;
  void* block = 0;
  if (tilemap.area != 0) {
    if (tilemap.area != area) {
      block = memory_realloc(tilemap.tiles, capacity);
    }
  } else {
    block = memory_alloc(capacity);
  }
  if (!block) {
    console_log("Error: tilemap_copy memory_alloc ERR_NOT_ENOUGH_MEMORY");
  }
  tilemap.tiles = block;
  memory_copy(tilemap.tiles, tiles, capacity);
  tilemap.width = width;
  tilemap.height = height;
  tilemap.area = area;
}
export void tilemap_set_player(f32 x, f32 y) {
  tilemap.offset.x = x - tilemap.visible_tiles[0] / 2 + .5f;
  tilemap.offset.y = y - tilemap.visible_tiles[1] / 2 + .5f;
}
export void tilemap_load() {
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
  tilemap_onresize();
  // loaded
  tilemap.loaded = true;
  window_updated = true;
}
export void tilemap_unload() {
  tilemap.loaded = false;
  assert(tilemap.area != 0);
  tilemap.area = 0;
  const u8 rendered_tiles = (tilemap.rendered_blocks[0] * 4) * (tilemap.rendered_blocks[1] * 4);
  vertices_reserve(vertices_capacity - rendered_tiles * 4 + TILEMAP_VERTICES_USED);
  indexes_reserve(indexes_capacity - rendered_tiles * 6 + TILEMAP_INDEXES_USED);
  assert(tilemap.tiles != 0);
  memory_free(tilemap.tiles);
}
export void tilemap_onresize() {
  tilemap.visible_tiles[0] = (f32)window_width / TILE_SIZE;
  tilemap.visible_tiles[1] = (f32)window_height / TILE_SIZE;
  const f32 rendered_blocks[2] = {
    math_ceil(tilemap.visible_tiles[0]) + 1,
    math_ceil(tilemap.visible_tiles[1]) + 1
  };
  if (rendered_blocks[0] != tilemap.rendered_blocks[0] || rendered_blocks[1] != tilemap.rendered_blocks[1]) {
    const u64 previous_rendered_tiles = (tilemap.rendered_blocks[0] * 4) * (tilemap.rendered_blocks[1] * 4);
    const u64 rendered_tiles = (rendered_blocks[0] * 4) * (rendered_blocks[1] * 4);
    vertices_reserve(
      vertices_capacity
      + rendered_tiles * 4 + TILEMAP_VERTICES_USED
      - previous_rendered_tiles * 4 + TILEMAP_VERTICES_USED
    );
    indexes_reserve(
      vertices_capacity
      + rendered_tiles * 6 + TILEMAP_INDEXES_USED
      - previous_rendered_tiles * 6 + TILEMAP_INDEXES_USED
    );
    tilemap.rendered_blocks[0] = rendered_blocks[0];
    tilemap.rendered_blocks[1] = rendered_blocks[1];
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
    }
    if (window_key_pressed(KEY_LEFT)) {
      tilemap_moveto(tilemap.offset.x - 1.f, tilemap.offset.y);
      tilemap.player_direction = KEY_LEFT;
    } else if (window_key_pressed(KEY_RIGHT)) {
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
  if (tilemap.area) {
    const i8 start_x = (i8)math_floor(tilemap.offset.x);
    const i8 start_y = (i8)math_floor(tilemap.offset.y);
    const i8 end_x = start_x + tilemap.rendered_blocks[0];
    const i8 end_y = start_y + tilemap.rendered_blocks[1];
    const f32 start_x0 = -1.f - (tilemap.offset.x - start_x) * tilemap.tile_ndc_pixel[0];
    const f32 start_y0 = 1.f + (tilemap.offset.y - start_y) * tilemap.tile_ndc_pixel[1];
    for (u8 layer = 0; layer < TILEMAP_LAYERS; layer++) {
      f32 x0, y0, x1, y1;
      if (!tilemap.player_hidden && layer == 2) {
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
          // block draw
          tileblock_t* block = (x >= 0 && x < tilemap.width) && (y >= 0 && y < tilemap.height)
            ? &tilemap_tile(layer, x, y)
            : 0;
          if (block != 0) {
            u8 tile_x;
            u8 tile_y;
            // top_left
            if (block->top_left.id) {
              tile_x = (block->top_left.id - 1) % 10;
              tile_y = math_ceil((f32)block->top_left.id / 10.f) - 1.f;
              tile_draw(
                x0,
                y0,
                x1 - tilemap.tile_ndc_pixel[0] / 2,
                y1 + tilemap.tile_ndc_pixel[1] / 2,
                tile_x, tile_y,
                block->top_left.flags
              );
            }
            // top_right
            if (block->top_right.id) {
              tile_x = (block->top_right.id - 1) % 10;
              tile_y = math_ceil((f32)block->top_right.id / 10.f) - 1.f;
              tile_draw(
                x0 + tilemap.tile_ndc_pixel[0] / 2,
                y0,
                x1,
                y1 + tilemap.tile_ndc_pixel[1] / 2,
                tile_x, tile_y,
                block->top_right.flags
              );
            }
            // bottom_left
            if (block->bottom_left.id) {
              tile_x = (block->bottom_left.id - 1) % 10;
              tile_y = math_ceil((f32)block->bottom_left.id / 10.f) - 1.f;
              tile_draw(
                x0,
                y0 - tilemap.tile_ndc_pixel[1] / 2,
                x1 - tilemap.tile_ndc_pixel[0] / 2,
                y1,
                tile_x, tile_y,
                block->bottom_left.flags
              );
            }
            // bottom_right
            if (block->bottom_right.id) {
              tile_x = (block->bottom_right.id - 1) % 10;
              tile_y = math_ceil((f32)block->bottom_right.id / 10.f) - 1.f;
              tile_draw(
                x0 + tilemap.tile_ndc_pixel[0] / 2,
                y0 - tilemap.tile_ndc_pixel[1] / 2,
                x1,
                y1,
                tile_x, tile_y,
                block->bottom_right.flags
              );
            }
          }
          y0 = y1;
        }
        x0 = x1;
      }
    }
  }
}