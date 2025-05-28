#include <game/game.h>
#include <game/tilemap.h>
#include <game/transition.scene.h>
#include <sdk/window.h>

#include <stdio.h>

#define TILEMAP_VERTICES_USED QUAD_VERTEX_COUNT
#define TILEMAP_INDEXES_USED QUAD_INDEX_COUNT

#define CHUNK_FILE_NAME "assets/world/tiles_-xxx_-yyy_-zzz.map"
#define CHUNK_FILE_FORMAT "assets/world/tiles_%hi_%hi_%hi.map"

tilemap_t* global_tilemap = 0;

void __tilemap_chunk_load(chunk_t* chunk) {
  if (chunk->z == MIN_I8)
    goto chunk_empty;
  char file_path [] = CHUNK_FILE_NAME;
  string_format(
    file_path, sizeof(file_path),
    CHUNK_FILE_FORMAT, chunk->x, chunk->y, chunk->z
  );
  FILE* file = fopen(file_path, "rb");
  if (!file) {
    error(ERR_NOT_FOUND, "__tilemap_chunk_load '%s'", file_path);
    goto chunk_error;
  }
  u64 result;
  result = fread(&chunk->tiles, sizeof(chunk->tiles), 1, file);
  if (result != sizeof(chunk->tiles))
    goto chunk_error;
  fclose(file);
chunk_error:
  window_updated = true;
  return;
chunk_empty:
  memory_fill(chunk->tiles, '\0', sizeof(chunk->tiles));
}
void tilemap_chunk_save(chunk_t* chunk) {
  char file_path [] = CHUNK_FILE_NAME;
  string_format(
    file_path, sizeof(file_path),
    CHUNK_FILE_FORMAT, chunk->x, chunk->y, chunk->z
  );
  FILE* file = fopen(file_path, "wb");
  if (!file) {
    error(ERR_NOT_FOUND, "__tilemap_chunk_save '%s'", file_path);
  }
  u64 result;
  result = fwrite(&chunk->tiles, sizeof(chunk->tiles), 1, file);
  assert(result);
  fclose(file);
}
void tilemap_chunk_load() {
  // center
  const i8 chunk_x = global_tilemap->x / CHUNK_SIZE;
  const i8 chunk_y = global_tilemap->y / CHUNK_SIZE;
  chunk_t* chunk = &global_tilemap->chunks[CHUNK_CENTER];
  chunk->x = chunk_x;
  chunk->y = chunk_y;
  chunk->z = global_tilemap->z;
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top-left
  chunk = &global_tilemap->chunks[CHUNK_TOP_LEFT];
  if (chunk_x > MIN_I8 && chunk_y > MIN_I8) {
    chunk->x = chunk_x - 1;
    chunk->y = chunk_y - 1;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top
  chunk = &global_tilemap->chunks[CHUNK_TOP];
  if (chunk_y < MAX_I8) {
    chunk->x = chunk_x;
    chunk->y = chunk_y + 1;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // top-right
  chunk = &global_tilemap->chunks[CHUNK_TOP_RIGHT];
  if (chunk_y > MIN_I8 && chunk_x < MAX_I8) {
    chunk->x = chunk_x + 1;
    chunk->y = chunk_y + 1;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // left
  chunk = &global_tilemap->chunks[CHUNK_LEFT];
  if (chunk_x > MIN_I8) {
    chunk->x = chunk_x - 1;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // right
  chunk = &global_tilemap->chunks[CHUNK_RIGHT];
  if (chunk_x < MAX_I8) {
    chunk->x = chunk_x + 1;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // bottom
  chunk = &global_tilemap->chunks[CHUNK_BOTTOM];
  if (chunk_y > MIN_I8) {
    chunk->x = chunk_x;
    chunk->y = chunk_y - 1;
    chunk->z = global_tilemap->z;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // up
  chunk = &global_tilemap->chunks[CHUNK_UP];
  if (global_tilemap->z < MAX_I8) {
    chunk->x = chunk_x;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z + 1;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
  // down
  if (global_tilemap->z < MIN_I8) {
    chunk = &global_tilemap->chunks[CHUNK_DOWN];
    chunk->x = chunk_x;
    chunk->y = chunk_y;
    chunk->z = global_tilemap->z - 1;
  } else {
    chunk->z = MIN_I8;
  }
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)__tilemap_chunk_load, chunk, 0);
}

void tilemap_load() {
  global_tilemap = memory_alloc0(sizeof(tilemap_t));
  tilemap_chunk_load();
  // global_tilemap
  global_tilemap->tile_size = 30;
  global_tilemap->player_direction = KEY_DOWN;
  window_background[0] = 0.f;
  window_background[1] = 0.f;
  window_background[2] = 0.f;
  tilemap_onresize();
  tilemap_move(0, 0);
  // loaded
  window_updated = true;
}
void tilemap_unload() {
  assert(global_tilemap != null);
  assert(global_tilemap->screen_tiles != null);
  vertices_reserve(vertices_capacity - global_tilemap->rendered_total * QUAD_VERTEX_COUNT + TILEMAP_VERTICES_USED);
  indexes_reserve(indexes_capacity - global_tilemap->rendered_total * QUAD_INDEX_COUNT + TILEMAP_INDEXES_USED);
  memory_free(global_tilemap->screen_tiles);
  memory_free(global_tilemap);
  global_tilemap = 0;
}
void tilemap_set_center(f32 x, f32 y) {
  global_tilemap->x = x - global_tilemap->visible_tiles_x / 2 + .5f;
  global_tilemap->y = y - global_tilemap->visible_tiles_y / 2 + .5f;
}
void tilemap_onresize() {
  global_tilemap->visible_tiles_x = (f32)window_width / global_tilemap->tile_size;
  global_tilemap->visible_tiles_y = (f32)window_height / global_tilemap->tile_size;
  const u8 rendered_tiles_x = math_ceil(global_tilemap->visible_tiles_x) + 2;
  const u8 rendered_tiles_y = math_ceil(global_tilemap->visible_tiles_y) + 2;
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
      ? global_tilemap->rendered_total * QUAD_VERTEX_COUNT + TILEMAP_VERTICES_USED
      : 0;
    vertices_reserve(
      vertices_capacity
      + rendered_total * QUAD_VERTEX_COUNT + TILEMAP_VERTICES_USED
      - previous_vertices
    );
    const u64 previous_indexes = global_tilemap->rendered_total
      ? global_tilemap->rendered_total * QUAD_INDEX_COUNT + TILEMAP_INDEXES_USED
      : 0;
    indexes_reserve(
      indexes_capacity
      + rendered_total * QUAD_INDEX_COUNT + TILEMAP_INDEXES_USED
      - previous_indexes
    );
    global_tilemap->rendered_tiles_x = rendered_tiles_x;
    global_tilemap->rendered_tiles_y = rendered_tiles_y;
    global_tilemap->rendered_area = rendered_area;
    global_tilemap->rendered_total = rendered_total;
  }
  global_tilemap->tile_ndc_pixel_x = global_tilemap->tile_size * window_pixel_ndc_x;
  global_tilemap->tile_ndc_pixel_y = global_tilemap->tile_size * window_pixel_ndc_y;
  window_updated = true;
}
tile_t* tile_from_screen() {
  f32 mouse_offset_x = math_round_epsilonf((f32)mouse_x / (f32)global_tilemap->tile_size);
  f32 mouse_offset_y = math_round_epsilonf((f32)mouse_y / (f32)global_tilemap->tile_size);
  f32 screen_offset_x = global_tilemap->x - math_floorf(global_tilemap->x);
  f32 screen_offset_y = global_tilemap->y - math_floorf(global_tilemap->y);
  u8 tile_x = math_floorf(screen_offset_x + mouse_offset_x);
  u8 tile_y = math_floorf(screen_offset_y + mouse_offset_y);
  return tilemap_tile(tile_x, tile_y, global_tilemap->z);
}
void tilemap_moveto(f32 x, f32 y, f32 duration) {
  global_tilemap->move_timer = 0;
  global_tilemap->move_duration = duration;
  global_tilemap->start_x = global_tilemap->x;
  global_tilemap->start_y = global_tilemap->y;
  global_tilemap->target_x = x;
  global_tilemap->target_y = y;
  global_tilemap->moving = true;
  window_updated = true;
}
void tilemap_onkeypress() {
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
void tilemap_move(f32 world_x, f32 world_y) {
  // camera_update
  global_tilemap->x = math_round_epsilonf(world_x);
  global_tilemap->y = math_round_epsilonf(world_y);
  // chunks->screen_tiles
  const chunk_t* chunk_center = &global_tilemap->chunks[CHUNK_CENTER];
  for (u8 y = 0; y < global_tilemap->rendered_tiles_y; y++) {
    for (u8 x = 0; x < global_tilemap->rendered_tiles_x; x++) {
      i32 world_x = math_floorf(global_tilemap->x + x);
      i32 world_y = math_floorf(global_tilemap->y + y);
      // chunk_find
      const i8 chunk_offset_x = (world_x - chunk_center->x) / CHUNK_SIZE;
      const i8 chunk_offset_y = (world_y - chunk_center->y) / CHUNK_SIZE;
      const u8 chunk_index = CHUNK_CENTER + chunk_offset_y * 3 + chunk_offset_x;
      assert(chunk_index < CHUNK_MAX);
      chunk_t* chunk = &global_tilemap->chunks[chunk_index];
      assert(chunk != null);
      // chunk_tile
      u8 chunk_tile_x = world_x % CHUNK_SIZE;
      u8 chunk_tile_y = world_y % CHUNK_SIZE;
      tile_t* chunk_tile = chunk->tiles[chunk_tile_x][chunk_tile_y];
      assert(chunk_tile != null);
      for (u8 layer = 0; layer < LAYER_MAX; layer++) {
        tilemap_tile(x, y, layer) = &chunk_tile[layer];
      }
    }
  }
  // render_request
  window_updated = true;
}
void tilemap_draw() {
  // moveto_update
  if (global_tilemap->moving) {
    global_tilemap->move_timer += window_deltatime;
    f32 progress = global_tilemap->move_timer / global_tilemap->move_duration;
    if (progress > 1.f) {
      progress = 1.f;
      global_tilemap->moving = false;
    }
    tilemap_move(
      (global_tilemap->target_x - global_tilemap->start_x) * progress + global_tilemap->start_x,
      (global_tilemap->target_y - global_tilemap->start_y) * progress + global_tilemap->start_y
    );
    window_updated = true;
  }
  // draw
  const f32 start_x0 = -1.f - (global_tilemap->x - math_floorf(global_tilemap->x)) * global_tilemap->tile_ndc_pixel_x;
  const f32 start_y0 = 1.f + (global_tilemap->y - math_floorf(global_tilemap->y)) * global_tilemap->tile_ndc_pixel_y;
  f32 x0 = 0;
  f32 y0 = 0;
  f32 x1 = 0;
  f32 y1 = 0;
  tile_t** screen_tile = global_tilemap->screen_tiles;
  for (u8 layer = 0; layer < LAYER_MAX; layer++) {
    // player_draw
    if (global_tilemap->player_hidden == false && layer == 2) {
      x0 = -global_tilemap->tile_ndc_pixel_x / 2;
      y0 = global_tilemap->tile_ndc_pixel_y / 2;
      x1 = -x0;
      y1 = -y0;
      window_rect_fill(
        x0, y0, x1, y1,
        1, 0, 0, 1
      );
    }
    // tile_draw
    y0 = start_y0;
    for (u8 y = 0; y < global_tilemap->rendered_tiles_y; y++) {
      x0 = start_x0;
      for (u8 x = 0; x < global_tilemap->rendered_tiles_x; x++) {
        x1 = x0 + global_tilemap->tile_ndc_pixel_x;
        y1 = y0 - global_tilemap->tile_ndc_pixel_y;
        tile_t tile = **screen_tile++;
        if (tile.id) {
          u8 tile_x = (tile.id - 1) % atlas_tiles_width;
          u8 tile_y = math_ceil((f32)tile.id / atlas_tiles_height) - 1.f;
          tile_draw(x0, y0, x1, y1, tile_x, tile_y, tile.flags);
        }
        x0 = x1;
      }
      y0 = y1;
    }
  }
}