#include <game/game.h>
#include <game/tilemap.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#include <stdio.h>

#define TILEMAP_VERTICES_USED 4
#define TILEMAP_INDEXES_USED 6

tilemap_t* global_tilemap = 0;

export void tilemap_chunk_save(chunk_t* chunk) {
  char file_path [] = "assets/world/tiles_xxx_yyy_zzz.map";
  string_format(
    file_path, sizeof(file_path),
    "assets/world/tiles_%u_%u_%u.map", chunk->x, chunk->y, chunk->z
  );
  FILE* file = fopen(file_path, "wb");
  if (!file) {
    console_log("__tilemap_chunk_save NOT FOUND '%s'", file_path);
  }
  u64 result;
  result = fwrite(&chunk->tiles, sizeof(chunk->tiles), 1, file);
  assert(result);
  result = fwrite(&chunk->warps_count, sizeof(chunk->warps_count), 1, file);
  assert(result);
  result = fwrite(chunk->warps, sizeof(warp_t), chunk->warps_count, file);
  assert(result);
  fclose(file);
}
export void __tilemap_chunk_load(chunk_t* chunk) {
  if (chunk->z == MAX_U8)
    goto chunk_null;
  chunk->x_end = chunk->x + CHUNK_SIZE;
  chunk->y_end = chunk->y + CHUNK_SIZE;
  char file_path [] = "assets/world/tiles_xxx_yyy_zzz.map";
  string_format(
    file_path, sizeof(file_path),
    "assets/world/tiles_%u_%u_%u.map", chunk->x, chunk->y, chunk->z
  );
  FILE* file = fopen(file_path, "rb");
  if (!file) {
    console_log("__tilemap_chunk_load NOT FOUND '%s'", file_path);
    goto chunk_error;
  }
  u64 result;
  result = fread(&chunk->tiles, sizeof(chunk->tiles), 1, file);
  if (result != sizeof(chunk->tiles))
    goto chunk_error;
  result = fread(&chunk->warps_count, sizeof(chunk->warps_count), 1, file);
  if (result != sizeof(chunk->warps_count))
    goto chunk_error;
  if (chunk->warps_count > 0) {
    if (chunk->warps != null)
      memory_free(chunk->warps);
    chunk->warps = memory_alloc(chunk->warps_count * sizeof(warp_t));
    if (chunk->warps == null)
      goto chunk_error;
    result = fread(chunk->warps, sizeof(warp_t), chunk->warps_count, file);
    if (result != sizeof(warp_t) * chunk->warps_count)
      goto chunk_error;
  }
  fclose(file);
chunk_error:
  window_updated = true;
  return;
chunk_null:
  memory_fill(chunk->tiles, '\0', sizeof(chunk->tiles));
  if (chunk->warps != null) {
    memory_free(chunk->warps);
    chunk->warps = null;
  }
}
export void tilemap_chunk_load() {
  // center
  const u16 chunk_x = global_tilemap->x / CHUNK_SIZE;
  const u16 chunk_y = global_tilemap->y / CHUNK_SIZE;
  chunk_t* chunk = &global_tilemap->chunks[CHUNK_CENTER];
  console_log("tilemap_chunk_load %p", chunk);
  chunk->x = chunk_x;
  chunk->y = chunk_y;
  chunk->z = global_tilemap->z;
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top-left
  chunk = &global_tilemap->chunks[CHUNK_TOP_LEFT];
  if (chunk_x > 0 && chunk_y > 0) {
    chunk->x = chunk_x - CHUNK_SIZE;
    chunk->y = chunk_y - CHUNK_SIZE;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top
  chunk = &global_tilemap->chunks[CHUNK_TOP];
  if (chunk_y > 0) {
    chunk->x = chunk_x;
    chunk->y = chunk_y - CHUNK_SIZE;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top-right
  chunk = &global_tilemap->chunks[CHUNK_TOP_RIGHT];
  if (chunk_y > 0 && chunk_x < MAX_U16) {
    chunk->x = chunk_x + CHUNK_SIZE;
    chunk->y = chunk_y - CHUNK_SIZE;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // left
  chunk = &global_tilemap->chunks[CHUNK_LEFT];
  if (chunk_x > 0) {
    chunk->x = chunk_x - CHUNK_SIZE;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // right
  chunk = &global_tilemap->chunks[CHUNK_RIGHT];
  if (chunk_x < MAX_U16) {
    chunk->x = chunk_x + CHUNK_SIZE;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // bottom
  chunk = &global_tilemap->chunks[CHUNK_BOTTOM];
  if (chunk_y < MAX_U8) {
    chunk->x = chunk_x;
    chunk->y = chunk_y + CHUNK_SIZE;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // up
  chunk = &global_tilemap->chunks[CHUNK_UP];
  if (global_tilemap->z < MAX_U8) {
    chunk->x = chunk_x;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z + 1;
  } else {
    chunk->z = MAX_U8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // down
  chunk = &global_tilemap->chunks[CHUNK_DOWN];
  chunk->x = chunk_x;
  chunk->y = chunk_y;
  chunk->z = global_tilemap->z - 1;
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
}

export void tilemap_load() {
  global_tilemap = memory_alloc0(sizeof(tilemap_t));
  tilemap_chunk_load();
  // global_tilemap
  global_tilemap->tile_size = 30;
  global_tilemap->player_direction = KEY_DOWN;
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
  tilemap_onresize();
  // loaded
  window_updated = true;
}
export void tilemap_unload() {
  assert(global_tilemap != null);
  assert(global_tilemap->screen_tiles != null);
  vertices_reserve(vertices_capacity - global_tilemap->rendered_total * 4 + TILEMAP_VERTICES_USED);
  indexes_reserve(indexes_capacity - global_tilemap->rendered_total * 6 + TILEMAP_INDEXES_USED);
  memory_free(global_tilemap->screen_tiles);
  memory_free(global_tilemap);
  global_tilemap = 0;
}
export void tilemap_camera_center(f32 x, f32 y) {
  global_tilemap->x = x - global_tilemap->visible_tiles[0] / 2 + .5f;
  global_tilemap->y = y - global_tilemap->visible_tiles[1] / 2 + .5f;
}
export void tilemap_onresize() {
  global_tilemap->visible_tiles[0] = (f32)window_width / global_tilemap->tile_size;
  global_tilemap->visible_tiles[1] = (f32)window_height / global_tilemap->tile_size;
  const u8 rendered_tiles_x = math_ceil(global_tilemap->visible_tiles[0]) + 2;
  const u8 rendered_tiles_y = math_ceil(global_tilemap->visible_tiles[1]) + 2;
  const u64 rendered_area = rendered_tiles_x * rendered_tiles_y;
  const u64 rendered_total = rendered_area * LAYER_MAX;
  if (global_tilemap->rendered_total != rendered_total) {
    if (global_tilemap->screen_tiles) {
      global_tilemap->screen_tiles = (tile_t**)memory_realloc0(global_tilemap->screen_tiles, rendered_total * sizeof(tile_t*));
      assert(global_tilemap != null);
    } else {
      global_tilemap->screen_tiles = (tile_t**)memory_alloc0(rendered_total * sizeof(tile_t*));
    }
    // vertices
    const u64 previous_vertices = global_tilemap->rendered_total
      ? global_tilemap->rendered_total * 4 + TILEMAP_VERTICES_USED
      : 0;
    vertices_reserve(
      vertices_capacity
      + rendered_total * 4 + TILEMAP_VERTICES_USED
      - previous_vertices
    );
    const u64 previous_indexes = global_tilemap->rendered_total
      ? global_tilemap->rendered_total * 6 + TILEMAP_INDEXES_USED
      : 0;
    indexes_reserve(
      indexes_capacity
      + rendered_total * 6 + TILEMAP_INDEXES_USED
      - previous_indexes
    );
    global_tilemap->rendered_tiles[0] = rendered_tiles_x;
    global_tilemap->rendered_tiles[1] = rendered_tiles_y;
    global_tilemap->rendered_area = rendered_area;
    global_tilemap->rendered_total = rendered_total;
  }
  global_tilemap->tile_ndc_pixel[0] = global_tilemap->tile_size * window_pixel_ndc[0];
  global_tilemap->tile_ndc_pixel[1] = global_tilemap->tile_size * window_pixel_ndc[1];
  window_updated = true;
}
export tile_t* tile_from_screen() {
  f32 mouse_offset_x = (f32)mouse_x / (f32)global_tilemap->tile_size + FLOAT_FIXER;
  f32 mouse_offset_y = (f32)mouse_y / (f32)global_tilemap->tile_size + FLOAT_FIXER;
  u8 tile_x = math_floorf(global_tilemap->x + mouse_offset_x);
  u8 tile_y = math_floorf(global_tilemap->y + mouse_offset_y);
  return tilemap_tile(tile_x, tile_y, global_tilemap->z);
}
export void tilemap_moveto(f32 x, f32 y, f32 duration) {
  global_tilemap->move_timer = 0;
  global_tilemap->move_duration = duration;
  global_tilemap->start_offset.x = global_tilemap->x;
  global_tilemap->start_offset.y = global_tilemap->y;
  global_tilemap->target_offset.x = x;
  global_tilemap->target_offset.y = y;
  global_tilemap->moving = true;
  window_updated = true;
}
export void tilemap_onkeypress() {
  if (global_tilemap->moving == false) {
    const f32 duration = .5f;
    if (window_key_pressed(KEY_UP)) {
      tilemap_moveto(global_tilemap->x, global_tilemap->y - 1.f, duration);
      global_tilemap->player_direction = KEY_UP;
    } else if (window_key_pressed(KEY_DOWN)) {
      tilemap_moveto(global_tilemap->x, global_tilemap->y + 1.f, duration);
      global_tilemap->player_direction = KEY_DOWN;
    }
    if (window_key_pressed(KEY_LEFT)) {
      tilemap_moveto(global_tilemap->x - 1.f, global_tilemap->y, duration);
      global_tilemap->player_direction = KEY_LEFT;
    } else if (window_key_pressed(KEY_RIGHT)) {
      tilemap_moveto(global_tilemap->x + 1.f, global_tilemap->y, duration);
      global_tilemap->player_direction = KEY_RIGHT;
    }
  }
}
export void tilemap_draw() {
  // chunks->screen_tiles
  chunk_t* chunk = &global_tilemap->chunks[CHUNK_CENTER];
  for (i8 y = 0; y < global_tilemap->rendered_tiles[1]; y++) {
    for (i8 x = 0; x < global_tilemap->rendered_tiles[0]; x++) {
      u16 world_x = global_tilemap->x + x;
      u16 world_y = global_tilemap->y + y;
      // chunk_find
      u16 chunk_x = world_x / CHUNK_SIZE;
      u16 chunk_y = world_y / CHUNK_SIZE;
      if (
        chunk->x != chunk_x
        || chunk->y != chunk_y
        || chunk->z != global_tilemap->z
        ) {
        for (u8 i = 0; i < CHUNK_MAX; i++) {
          chunk_t* find = &global_tilemap->chunks[i];
          if (
            find->x == chunk_x
            && find->y == chunk_y
            && find->z == global_tilemap->z
            ) {
            chunk = find;
            break;
          }
        }
      }
      u16 chunk_tile_x = world_x % CHUNK_SIZE;
      u16 chunk_tile_y = world_y % CHUNK_SIZE;
      tile_t* chunk_tile = chunk->tiles[chunk_tile_x][chunk_tile_y];
      for (u8 layer = 0; layer < LAYER_MAX; layer++) {
        tilemap_tile(x, y, layer) = &chunk_tile[layer];
      }
    }
  }
  // moveto_update
  if (global_tilemap->moving) {
    global_tilemap->move_timer += window_deltatime;
    f32 progress = global_tilemap->move_timer / global_tilemap->move_duration;
    if (progress > 1.f) {
      progress = 1.f;
      global_tilemap->moving = false;
    }
    global_tilemap->x = (global_tilemap->target_offset.x - global_tilemap->start_offset.x) * progress + global_tilemap->start_offset.x;
    global_tilemap->y = (global_tilemap->target_offset.y - global_tilemap->start_offset.y) * progress + global_tilemap->start_offset.y;
    if (global_tilemap->x < 0)
      global_tilemap->x = 0;
    if (global_tilemap->y < 0)
      global_tilemap->y = 0;
    window_updated = true;
  }
  // draw
  const f32 start_x0 = -1.f - (global_tilemap->x - (u8)global_tilemap->x) * global_tilemap->tile_ndc_pixel[0];
  const f32 start_y0 = 1.f + (global_tilemap->y - (u8)global_tilemap->y) * global_tilemap->tile_ndc_pixel[1];
  u64 tile_index = 0;
  f32 x0 = start_x0;
  f32 y0 = start_y0;
  f32 x1 = 0;
  f32 y1 = 0;
  for (u8 z = 0; z < LAYER_MAX; z++) {
    // player_draw
    if (global_tilemap->player_hidden == false && z == 2) {
      x0 = -global_tilemap->tile_ndc_pixel[0] / 2;
      y0 = global_tilemap->tile_ndc_pixel[1] / 2;
      x1 = -x0;
      y1 = -y0;
      window_rect_fill(
        x0, y0, x1, y1,
        1, 0, 0, 1
      );
    }
    // tile_draw
    y0 = start_y0;
    for (i8 y = 0; y < global_tilemap->rendered_tiles[1]; y++) {
      x0 = start_x0;
      for (i8 x = 0; x < global_tilemap->rendered_tiles[0]; x++) {
        x1 = x0 + global_tilemap->tile_ndc_pixel[0];
        y1 = y0 - global_tilemap->tile_ndc_pixel[1];
        tile_t* tile = global_tilemap->screen_tiles[tile_index++];
        if (tile && tile->id) {
          u8 tile_x = (tile->id - 1) % atlas_tiles_width;
          u8 tile_y = math_ceil((f32)tile->id / atlas_tiles_height) - 1.f;
          tile_draw(x0, y0, x1, y1, tile_x, tile_y, tile->flags);
        }
        x0 = x1;
      }
      y0 = y1;
    }
  }
}